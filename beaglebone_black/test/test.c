#include "jpeg.h"

static __init int tachy_jpeg_init (void) {
    int result;
    
    tachy_jpeg_dev_ptr = NULL;

    // register platform device, probe func is called by this func
    if ((result = platform_driver_register(&tachy_jpeg_driver)) < 0) {
        printk(KERN_ERR"[Tachy JPEG] Registring Platform Driver is Failed\n");
    } else {
        printk(KERN_INFO"[Tachy JPEG] Tachy JPEG is Initialized\n");
    }

    return result;
}

static __exit void tachy_jpeg_exit (void) {
    // unregister platform device, remove func is called by this func
    printk(KERN_INFO"[Tachy JPEG] EXIT CALLED\n");
    platform_driver_unregister(&tachy_jpeg_driver);
}

static int tachy_jpeg_probe (struct platform_device *pdev) {
    int                     i, j;
    struct resource         *tachy_jpeg_resource;
    char                    *label;
    struct tachy_jpeg_dev   *tmp_dev;

    // GFP_KERNEL : sleep if not enough memory managed by kernel
    // GFP_ATOMIC : return NULL if not enough memory managed by kernel
    // GFP_DMA    : allocate memory sequently for DMA controller
    if (!(tmp_dev = kzalloc(sizeof(struct tachy_jpeg_dev), GFP_KERNEL))) {
        printk(KERN_ERR"[Tachy JPEG] Unable to Allocate Memory for Device Structure. [%s]\n", pdev->name);
        return -ENOMEM;
    } else {
        printk(KERN_INFO"[Tachy JPEG] Allocated Memory for Device Structure. [%s]\n", pdev->name);
    }

    tmp_dev->pdev = pdev;

    if (tachy_jpeg_dev_ptr) {
        printk(KERN_INFO"[Tachy JPEG] pdev id is not started from 0\n");
    } else {
        pdev->id = 0;
    }

    tmp_dev->num_of_irq = platform_irq_count(pdev);

    if (!(tmp_dev->num_of_irq)) {
        printk(KERN_ERR"[Tachy JPEG] NO IRQ\n");
        kfree(tmp_dev);
        return -ENODEV;
    }

    // get resource of 0 idx of pdev
    // resource struct has phy start, end, flags
    if (!(tachy_jpeg_resource = platform_get_resource(pdev, IORESOURCE_MEM, 0))) {
        printk(KERN_ERR"[Tachy JPEG] No Memory Range\n");
        kfree(tmp_dev);
        return -ENODEV;
    } else if ((tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1) < tmp_dev->num_of_irq) {
        printk(KERN_ERR"[Tachy JPEG] Invalid Range\n");
        kfree(tmp_dev);
        return -ENODEV; 
    } else {
        tmp_dev->phy_start = tachy_jpeg_resource->start;
        tmp_dev->phy_end = tachy_jpeg_resource->end;
        // alloc memory from start address of resource to end address as named of DEVICE_NAME
        if (!request_mem_region(tachy_jpeg_resource->start, (tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1), DEVICE_NAME)) {
            printk(KERN_ERR"[Tachy JPEG] Failed to request Memory\n");
            kfree(tmp_dev);
            return -ENOMEM;
        }

        if (!(tmp_dev->virt_start = ioremap(tachy_jpeg_resource->start, (tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1)))) {
            printk(KERN_ERR"[Tachy JPEG] Mapping Memory is Failed\n");
            release_mem_region(tmp_dev->phy_start, tmp_dev->num_of_irq * (tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1));
            kfree(tmp_dev);
        }
    }

    // alloc for isr
    if (!(tmp_dev->isr_params = kzalloc(tmp_dev->num_of_irq * sizeof(struct isr_params), GFP_ATOMIC))) {
        printk(KERN_ERR"[Tachy JPEG] Unable to Allocate Memroy for ISR_PARAM\n");
        release_mem_region(tmp_dev->phy_start, tmp_dev->num_of_irq * (tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1));
        iounmap(tmp_dev->virt_start);
        kfree(tmp_dev);
        return -ENOMEM;
    }

    // alloc for irqs
    if (!(tmp_dev->irqs = kzalloc(tmp_dev->num_of_irq * sizeof(int), GFP_KERNEL))) {
        printk(KERN_ERR"[Tachy JPEG] Unable to Allocate Memory for IRQ\n");
        release_mem_region(tmp_dev->phy_start, tmp_dev->num_of_irq * (tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1));
        iounmap(tmp_dev->virt_start);
        kfree(tmp_dev->isr_params);
        kfree(tmp_dev);
        return -ENOMEM;
    }

    if (!(tmp_dev->write_works = kzalloc(tmp_dev->num_of_irq * sizeof(struct jpeg_delay_work), GFP_KERNEL))) {
        printk(KERN_ERR"[Tachy JPEG] Unable to Allocate Memory for Writing\n");
        release_mem_region(tmp_dev->phy_start, tmp_dev->num_of_irq * (tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1));
        iounmap(tmp_dev->virt_start);
        kfree(tmp_dev->isr_params);
        kfree(tmp_dev->irqs);
        kfree(tmp_dev);
        return -ENOMEM;
    }

    if (!(tmp_dev->work_queue = kzalloc(tmp_dev->num_of_irq * sizeof(struct workqueue_struct*), GFP_KERNEL))) {
        printk(KERN_ERR"[Tachy JPEG] Failed to Allocate Work Queue\n");
        release_mem_region(tmp_dev->phy_start, tmp_dev->num_of_irq * (tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1));
        iounmap(tmp_dev->virt_start);
        kfree(tmp_dev->isr_params);
        kfree(tmp_dev->irqs);
        kfree(tmp_dev->write_works);
        kfree(tmp_dev);
        return -ENOMEM;
    }

    for (i = 0; i < tmp_dev->num_of_irq; i++) {
        label = kasprintf(GFP_KERNEL, "JPEG_WORKQUEUE_%d_%d", pdev->id, i);
        if (!(tmp_dev->work_queue[i] = create_workqueue(label))) {
            printk(KERN_ERR"[Tachy JPEG] Failed to Create Work Queue\n");
            for (j = 0; j < i; j++) {
                destroy_workqueue(tmp_dev->work_queue[j]);
            }
            release_mem_region(tmp_dev->phy_start, tmp_dev->num_of_irq * (tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1));
            iounmap(tmp_dev->virt_start);
            kfree(tmp_dev->isr_params);
            kfree(tmp_dev->irqs);
            kfree(tmp_dev->write_works);
            kfree(label);
            kfree(tmp_dev);
            return -ENOMEM;
        }
        kfree(label);    
    }

    tmp_dev->waits = kzalloc(sizeof(wait_queue_head_t) * tmp_dev->num_of_irq, GFP_KERNEL);
    for (i = 0; i < tmp_dev->num_of_irq; i++) {
        //INIT_DELAYED_WORK(&(tmp_dev->write_works[i].work), write_reg);
        init_waitqueue_head(&(tmp_dev->waits[i]));
        if ((tmp_dev->irqs[i] = platform_get_irq(pdev, i)) < 0) {
            printk(KERN_ERR"[Tachy JPEG] Invalid IRQ\n");
            for (j = 0; j < i - 1; j++) {
                free_irq(tmp_dev->irqs[j], &(tmp_dev->waits[j]));
            }
            release_mem_region(tmp_dev->phy_start, tmp_dev->num_of_irq * (tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1));
            iounmap(tmp_dev->virt_start);
            kfree(tmp_dev->isr_params);
            kfree(tmp_dev->irqs);
            kfree(tmp_dev->write_works);
            for (j = 0; j < tmp_dev->num_of_irq; j++) {
                destroy_workqueue(tmp_dev->work_queue[j]);
            }
            kfree(tmp_dev->work_queue);
            kfree(tmp_dev);
            return -ENOMEM;
        }
        tmp_dev->isr_params[i].flag = 0;
        tmp_dev->isr_params[i].wait = &(tmp_dev->waits[i]);
        if (request_irq(tmp_dev->irqs[i], tachy_jpeg_done, IRQF_TRIGGER_RISING, "tachy-jpeg", &(tmp_dev->isr_params[i])) < 0) {
            printk(KERN_ERR"[Tachy JPEG] Invalid IRQ\n");
            for (j = 0; j < i; j++) {
                free_irq(tmp_dev->irqs[j], &(tmp_dev->isr_params[j]));
            }
            release_mem_region(tmp_dev->phy_start, tmp_dev->num_of_irq * (tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1));
            iounmap(tmp_dev->virt_start);
            kfree(tmp_dev->isr_params);
            kfree(tmp_dev->irqs);
            kfree(tmp_dev->write_works);
            for (j = 0; j < tmp_dev->num_of_irq; j++) {
                destroy_workqueue(tmp_dev->work_queue[j]);
            }
            kfree(tmp_dev->work_queue);
            kfree(tmp_dev);
            return -ENODEV;
        }
    }

    label = kasprintf(GFP_KERNEL, "%s-%d", DEVICE_NAME, pdev->id);

    // alloc character device
    if (alloc_chrdev_region(&(tmp_dev->devno), 0, tmp_dev->num_of_irq, label) < 0) {
        printk(KERN_ERR"[Tachy JPEG] Failed to Allocate Character Device\n");
        
        release_mem_region(tmp_dev->phy_start, tmp_dev->num_of_irq * (tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1));
        iounmap(tmp_dev->virt_start);
        kfree(tmp_dev->write_works);
        kfree(tmp_dev->isr_params);
        for (i = 0; i < tmp_dev->num_of_irq; i++) {
            free_irq(tmp_dev->irqs[i], &(tmp_dev->isr_params[i]));
            destroy_workqueue(tmp_dev->work_queue[i]);
        }
        kfree(tmp_dev->work_queue);
        kfree(tmp_dev);
        kfree(label);
        return -ENODEV;
    }

    if (!(tmp_dev->dev_class = class_create(THIS_MODULE, label))) {
        printk(KERN_ERR"[Tachy JPEG] Failed to create class\n");
        kfree(label);
        unregister_chrdev_region(tmp_dev->devno, tmp_dev->num_of_irq);
        for (i = 0; i < tmp_dev->num_of_irq; i++) {
            free_irq(tmp_dev->irqs[i], &(tmp_dev->isr_params[i]));
            destroy_workqueue(tmp_dev->work_queue[i]);
        }
        release_mem_region(tmp_dev->phy_start, tmp_dev->num_of_irq * (tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1));
        iounmap(tmp_dev->virt_start);
        kfree(tmp_dev->work_queue);
        kfree(tmp_dev->isr_params);
        kfree(tmp_dev->write_works);
        kfree(tmp_dev);
        kfree(label);

        return -ENODEV;
    }

    kfree(label);

    // register file opreations of character device
    cdev_init(&(tmp_dev->cdev), &tachy_jpeg_ops);
    if (cdev_add(&(tmp_dev->cdev), tmp_dev->devno, tmp_dev->num_of_irq) < 0) {
        printk(KERN_ERR"[Tachy JPEG] Failed to Add CDEV\n");
        class_destroy(tmp_dev->dev_class);
        unregister_chrdev_region(tmp_dev->devno, tmp_dev->num_of_irq);
        release_mem_region(tmp_dev->phy_start, tmp_dev->num_of_irq * (tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1));
        iounmap(tmp_dev->virt_start);
        kfree(tmp_dev);
        for (i = 0; i < tmp_dev->num_of_irq; i++) {
            free_irq(tmp_dev->irqs[i], &(tmp_dev->isr_params[i]));
            destroy_workqueue(tmp_dev->work_queue[i]);
        }
        kfree(tmp_dev->irqs);
        kfree(tmp_dev->write_works);
        kfree(tmp_dev->isr_params);
        kfree(tmp_dev->work_queue);
        kfree(tmp_dev);
    
        return -ENODEV;
    }

    for (i = 0; i < tmp_dev->num_of_irq; i++) {
        label = kasprintf(GFP_KERNEL, "%s", DEVICE_NAME);
        if (!device_create(tmp_dev->dev_class, NULL, MKDEV(MAJOR(tmp_dev->devno), i), "%s", label)) {
            printk(KERN_ERR"[Tachy JPEG] Failed to Create Device\n");
            for (j = 0; j < i; j++) {
                device_destroy(tmp_dev->dev_class, MKDEV(MAJOR(tmp_dev->devno), j));
            }
            cdev_del(&(tmp_dev->cdev));
            class_destroy(tmp_dev->dev_class);
            kfree(label);
            unregister_chrdev_region(tmp_dev->devno, tmp_dev->num_of_irq);
            release_mem_region(tmp_dev->phy_start, tmp_dev->num_of_irq * (tachy_jpeg_resource->end - tachy_jpeg_resource->start + 1));
            iounmap(tmp_dev->virt_start);
            kfree(tmp_dev->isr_params);
            for (j = 0; j < tmp_dev->num_of_irq; j++) {
                free_irq(tmp_dev->irqs[j], &(tmp_dev->isr_params[j]));
                destroy_workqueue(tmp_dev->work_queue[j]);
            }
            kfree(tmp_dev->irqs);
            kfree(tmp_dev->write_works);
            kfree(tmp_dev->work_queue);
            kfree(tmp_dev);
            
            return -ENODEV;
        }
        kfree(label);
    }

    tachy_jpeg_dev_ptr = tmp_dev;
    
    return 0;
}

static int tachy_jpeg_remove (struct platform_device *pdev) {
    int i;
    struct tachy_jpeg_dev *tmp_dev;

    tmp_dev = tachy_jpeg_dev_ptr;
    printk(KERN_INFO"[Tachy JPEG] remove called %d\n", tmp_dev->num_of_irq);
    for (i = 0; i < tmp_dev->num_of_irq; i++) {
        printk(KERN_INFO"[Tachy JPEG] remove called 0\n");
        printk(KERN_INFO"[Tachy JPEG] remove called 1\n");
        free_irq(tmp_dev->irqs[i], &(tmp_dev->isr_params[i]));
        printk(KERN_INFO"[Tachy JPEG] remove called 2\n");
        device_destroy(tmp_dev->dev_class, MKDEV(MAJOR(tmp_dev->devno), i));
        printk(KERN_INFO"[Tachy JPEG] remove called 3\n");
        flush_workqueue(tmp_dev->work_queue[i]);
        printk(KERN_INFO"[Tachy JPEG] remove called 4\n");
        destroy_workqueue(tmp_dev->work_queue[i]);
        printk(KERN_INFO"[Tachy JPEG] remove called 5\n");
    }
    printk(KERN_INFO"[Tachy JPEG] remove called 6\n");
    kfree(tmp_dev->work_queue);
    cdev_del(&(tmp_dev->cdev));
    class_destroy(tmp_dev->dev_class);
    unregister_chrdev_region(tmp_dev->devno, tmp_dev->num_of_irq);
    release_mem_region(tmp_dev->phy_start, tmp_dev->num_of_irq * (tmp_dev->phy_end - tmp_dev->phy_start + 1));
    iounmap(tmp_dev->virt_start);
    kfree(tmp_dev->irqs);
    kfree(tmp_dev->write_works);
    kfree(tmp_dev->isr_params);
    kfree(tmp_dev);
    kfree(mapped);

    return 0 ;
}

static irqreturn_t tachy_jpeg_done (int irq, void *dev_id) {
    struct isr_params *params;

    params = dev_id;
    
    params->flag = 1;
    wake_up_interruptible(params->wait);
    printk(KERN_INFO"[Tachy JPEG] irq done\n");

    return IRQ_HANDLED;
}
    
       
static int tachy_jpeg_open (struct inode *indoe, struct file *filep) {

    return 0;
}

static int tachy_jpeg_release (struct inode *inode, struct file *filep) {
    

    return 0;
}

static ssize_t tachy_jpeg_read (struct file *filep, char *buf, size_t count, loff_t *f_pos) {
    int  ret = 0;
    u_int32_t  output_y_size  = count * sizeof(u_int8_t) / 3 * 2;
    u_int32_t  output_uv_size = count * sizeof(u_int8_t) / 3 / 2;
    printk(KERN_INFO"[Tachy JPEG] read start\n");

    //ret = copy_to_user((void __user*)buf, tachy_jpeg_dev_ptr->out_y_start, output_y_size);
    //if (ret) {
    //    printk(KERN_ERR"[Tachy JPEG] copy_to_user y addr ret : %d\n", ret);
    //}
    //ret = copy_to_user((void __user*)(buf + output_y_size), tachy_jpeg_dev_ptr->out_u_start, output_uv_size);
    //if (ret) {
    //    printk(KERN_ERR"[Tachy JPEG] copy_to_user u addr ret : %d\n", ret);
    //}
    //ret = copy_to_user((void __user *)(buf + output_y_size + output_uv_size) , tachy_jpeg_dev_ptr->out_v_start, output_uv_size);
    //if (ret) {
    //    printk(KERN_ERR"[Tachy JPEG] copy_to_user v addr ret : %d\n", ret);
    //}

    //int i = 0, j = 0;
    //for (i = 0; i < 32; i++) {
    //    for (j = 0; j < 32; j++) {
    //        printk(KERN_INFO"%02x ", *(u_int8_t*)(tachy_jpeg_dev_ptr->out_y_start + (i*32+j)));
    //    }
    //    printk(KERN_INFO"\n");
    //}

    //// clear decoder
    writel(0x40000000, tachy_jpeg_dev_ptr->virt_start);

    ////kfree(tachy_jpeg_dev_ptr->out_y_start);
    ////kfree(tachy_jpeg_dev_ptr->out_u_start);
    ////kfree(tachy_jpeg_dev_ptr->out_v_start);

    return 0;
}

static ssize_t tachy_jpeg_write (struct file *filep, const char *buf, size_t count, loff_t *f_pos) {
    int       ret = 0;
    u_int32_t position = 0; 
    u_int32_t idx_of_qt = 0;
    u_int32_t num_of_qt = 0;
    u_int32_t num_of_hf = 0;
    u_int32_t qt_reg_value = 0;
    u_int32_t hf_reg_value = 0;
    u_int32_t len_of_hf = 0;
    u_int32_t inner_loop_cnt = 0;
    u_int32_t idx_of_hf_cnt = 0;
    u_int32_t hv_size = 0;
    u_int32_t h_size = 0;
    u_int32_t v_size = 0;
    u_int32_t num_of_comp = 0;
    u_int32_t qt_idx_of_comp = 0;
    u_int32_t pos_of_dc0 = 0;
    u_int32_t pos_of_ac0 = 0;
    u_int32_t pos_of_dc1 = 0;
    u_int32_t pos_of_ac1 = 0;
    unsigned long node;

    dma_handler_in = dma_map_single((struct device*)tachy_jpeg_dev_ptr->pdev, mapped, count, DMA_TO_DEVICE);
    if (dma_mapping_error((struct device*)tachy_jpeg_dev_ptr->pdev, dma_handler_in)) {
        printk(KERN_ERR"[Tachy JPEG] mapping input data error\n");
        return -1;
    }

    while (position < count - 1) {
        if (*((u_int8_t*)mapped + position) == 0xff) {
            if (*((u_int8_t*)mapped + (position + 1)) == 0xdb) { //DQT
                position += 5;  // skip DQT marker, LQ / PQ/ TQ
                inner_loop_cnt = 0;
                while (*((u_int8_t*)mapped + (position)) != 0xff) {
                    qt_reg_value = (idx_of_qt << 30) | (inner_loop_cnt << 16) | *((u_int8_t*)mapped+position); 
                    writel(qt_reg_value, (void*)tachy_jpeg_dev_ptr->virt_start + DQT_REG_OFFSET); 
                    writel(0x1, (void*)tachy_jpeg_dev_ptr->virt_start + DQT_VLD_REG_OFFSET); 
                    position += 1;
                    inner_loop_cnt += 1;
                }
                idx_of_qt += 1;
            } else if (*((u_int8_t*)mapped + (position + 1)) == 0xc0) { //SOF
                position += 5;  // jump to height size 
                hv_size = *((u_int8_t*)(mapped + position)) << 24     | 
                          *((u_int8_t*)(mapped + position + 1)) << 16 | 
                          *((u_int8_t*)(mapped + position + 2)) << 8  | 
                          *((u_int8_t*)(mapped + position + 3));
                h_size = hv_size >> 16;
                v_size = hv_size & 0x0000ffff;
                writel(hv_size, (void*)tachy_jpeg_dev_ptr->virt_start + HV_SIZE_REG_OFFSET);
                num_of_comp = *((u_int8_t*)mapped  + (position + 4));
                if (num_of_comp == 3) {
                    writel(0x00000229, (void*)tachy_jpeg_dev_ptr->virt_start + COMP_REG_OFFSET);
                }
                position += 4;  // jump to num of component
                num_of_qt = *((u_int8_t*)mapped + position);  
                inner_loop_cnt = 0;
                while (inner_loop_cnt < num_of_qt) {
                    position += 3;
                    qt_idx_of_comp |= *((u_int8_t*)mapped + position) << (6 - inner_loop_cnt * 2);
                    inner_loop_cnt += 1;
                }
                writel(qt_idx_of_comp, (void*)tachy_jpeg_dev_ptr->virt_start + DQT_IDX_REG_OFFSET);
                printk(KERN_INFO"qt idx %08x\n", qt_idx_of_comp);
            } else if (*((u_int8_t*)mapped + (position + 1)) == 0xc4) { //DHT
                inner_loop_cnt = 0;
                len_of_hf = (*(((u_int8_t*)mapped + position + 2)) << 2 | 
                             *(((u_int8_t*)mapped + position + 3))) + 2;   // add ffc4
                if (num_of_hf == 0) {
                    pos_of_dc0 = idx_of_hf_cnt + 5;
                } else if (num_of_hf == 1) {
                    pos_of_ac0 = idx_of_hf_cnt + 5;
                } else if (num_of_hf == 2) {
                    pos_of_dc1 = idx_of_hf_cnt + 5;
                } else if (num_of_hf == 3) {
                    pos_of_ac1 = idx_of_hf_cnt + 5;
                }
                
                while (inner_loop_cnt < len_of_hf) {
                    hf_reg_value = idx_of_hf_cnt << 16 | *((u_int8_t*)mapped + position);
                    writel(hf_reg_value, (void*)tachy_jpeg_dev_ptr->virt_start + HF_REG_OFFSET);
                    writel(0x1, (void*)tachy_jpeg_dev_ptr->virt_start + HF_VLD_REG_OFFSET);
                    position += 1;
                    inner_loop_cnt += 1;
                    idx_of_hf_cnt += 1;
                }
                num_of_hf += 1;
            } else if (*((u_int8_t*)mapped + (position + 1)) == 0xda) { //SOS
                writel(virt_to_phys(mapped) + position, (void*)tachy_jpeg_dev_ptr->virt_start + SOS_STR_REG_OFFSET);
                break;  // doesn't need to be read after sos mark
            } else {
                position += 1;
            }
        } else {
            position += 1;
        }
    }

    writel(0x00000020, (void*)tachy_jpeg_dev_ptr->virt_start);  // table gen end 

    writel(pos_of_dc0 << 16 | pos_of_ac0, ((void*)tachy_jpeg_dev_ptr->virt_start + HF_COMP0_REG_OFFSET));
    writel(pos_of_dc1 << 16 | pos_of_ac1, ((void*)tachy_jpeg_dev_ptr->virt_start + HF_COMP1_REG_OFFSET));
    printk(KERN_INFO"dc 0 %08x\n", pos_of_dc0 << 16 | pos_of_ac0);
    printk(KERN_INFO"dc 1 %08x\n", pos_of_dc1 << 16 | pos_of_ac1);

    dma_unmap_single((struct device*)tachy_jpeg_dev_ptr->pdev, dma_handler_in, count, DMA_TO_DEVICE);

    //tachy_jpeg_dev_ptr->out_y_start = kmalloc(h_size * v_size, GFP_DMA);
    tachy_jpeg_dev_ptr->out_y_start = (void*)(mapped + count + 1);
    if (!tachy_jpeg_dev_ptr->out_y_start) {
        printk(KERN_ERR"[Tachy JPEG] failed to allocate out y\n");
        return 0;
    }
    dma_handler_y = dma_map_single((struct device*)tachy_jpeg_dev_ptr->pdev, tachy_jpeg_dev_ptr->out_y_start, h_size * v_size, DMA_FROM_DEVICE);
    if (dma_mapping_error((struct device*)tachy_jpeg_dev_ptr->pdev, dma_handler_y)) {
        printk(KERN_ERR"[Tachy JPEG] failed to map Y\n");
    }

    //tachy_jpeg_dev_ptr->out_u_start = kmalloc(h_size * v_size / 4, GFP_DMA);
    tachy_jpeg_dev_ptr->out_u_start = (void*)(mapped + count + (h_size * v_size) + 1);
    if (!tachy_jpeg_dev_ptr->out_u_start) {
        printk(KERN_ERR"[Tachy JPEG] failed to allocate out u\n");
        return 0;
    }
    dma_handler_u = dma_map_single((struct device*)tachy_jpeg_dev_ptr->pdev, tachy_jpeg_dev_ptr->out_u_start, h_size * v_size / 4, DMA_FROM_DEVICE);
    if (dma_mapping_error((struct device*)tachy_jpeg_dev_ptr->pdev, dma_handler_u)) {
        printk(KERN_ERR"[Tachy JPEG] failed to map U\n");
    }

    //tachy_jpeg_dev_ptr->out_v_start = kmalloc(h_size * v_size / 4, GFP_DMA);
    tachy_jpeg_dev_ptr->out_v_start = (void*)(mapped + count + (h_size * v_size) + (h_size * v_size / 4) + 1);
    if (!tachy_jpeg_dev_ptr->out_v_start) {
        printk(KERN_ERR"[Tachy JPEG] failed to allocate out v\n");
        return 0;
    }
    dma_handler_v = dma_map_single((struct device*)tachy_jpeg_dev_ptr->pdev, tachy_jpeg_dev_ptr->out_v_start, h_size * v_size / 4, DMA_FROM_DEVICE);
    if (dma_mapping_error((struct device*)tachy_jpeg_dev_ptr->pdev, dma_handler_v)) {
        printk(KERN_ERR"[Tachy JPEG] failed to map V\n");
    }

    writel(virt_to_phys(tachy_jpeg_dev_ptr->out_y_start), ((void*)tachy_jpeg_dev_ptr->virt_start + OUT_Y_REG_OFFSET));
    writel(virt_to_phys(tachy_jpeg_dev_ptr->out_u_start), ((void*)tachy_jpeg_dev_ptr->virt_start + OUT_U_REG_OFFSET));
    writel(virt_to_phys(tachy_jpeg_dev_ptr->out_v_start), ((void*)tachy_jpeg_dev_ptr->virt_start + OUT_V_REG_OFFSET));

    writel(0x20000001, ((void*)tachy_jpeg_dev_ptr->virt_start));

    node = (unsigned long) iminor(filep->f_inode);
    queue_delayed_work(tachy_jpeg_dev_ptr->work_queue[node], &(tachy_jpeg_dev_ptr->write_works[node].work), 0);
    tachy_jpeg_dev_ptr->isr_params[node].flag = 0;
    printk(KERN_INFO"[Tachy JPEG] Wait irq\n");
    wait_event_interruptible((tachy_jpeg_dev_ptr->waits[node]), tachy_jpeg_dev_ptr->isr_params[node].flag != 0);

    return 0;
}

static int tachy_jpeg_mmap (struct file *filep, struct vm_area_struct *vma) {
    size_t size = vma->vm_end - vma->vm_start;

    mapped = kmalloc(size * sizeof(u_int8_t), GFP_DMA); 
    if (!mapped) {
        printk(KERN_ERR"[Tachy JPEG] failed to map input img address\n");
        return 0;
    }

    return remap_pfn_range(vma, vma->vm_start, virt_to_phys(mapped)>>PAGE_SHIFT, size, vma->vm_page_prot);
}
    
static long tachy_jpeg_ioctl (struct file *filep, u_int32_t cmd, unsigned long arg) {
    int ret = -1;
    u_int32_t tmp = 0;

    switch (cmd) {
        case JDEC_IOCTL_SET_ADDR :
            ret = copy_from_user(jdec_ch_addr, (void __user *)arg, sizeof(u_int32_t) * 4);
            if (ret) {
                printk(KERN_ERR"[Tachy JPEG] failed to set address : %d\n", ret);
            }
            break;
        default :
            break;
    }

    return 0;
}
