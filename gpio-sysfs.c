#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>
#include<linux/of_device.h>


#define RDONLY 0x1
#define WRONLY 0x10
#define RDWR   0x11

#define MAX_DEVICES 10

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt, __func__

/* Device private data structure */
struct gpiodev_private_data
{
  char label[20];
};

/* Driver private data structure */
struct gpiodrv_private_data
{
  int total_devices;
  struct class *class_gpio;
};

/* Remove function of GPIO driver */
int gpio_remove(struct platform_device *gpio_dev)
{
  return 0;
}

/* Probe function of GPIO driver */
int gpio_probe(struct platform_device *gpio_dev)
{
  return 0;
}

/* Match table for device compatible matching */
struct of_device_id gpiodrv_dt_match[] = {
  {.compatible = "org,bone-gpio-sysfs"},
  {} /* NULL terminated end */
};

/* Instance of paltform driver representing structure */
struct platform_driver gpio_platform_driver = {
  .probe = gpio_probe,
  .remove = gpio_remove,
  .driver = {
    .name = "bone-gpio-ysfs",
    .of_match_table = of_match_ptr(gpiodrv_dt_match)
  }

};

/* Variable of private data structure*/
struct gpiodrv_private_data gpiodrv_private_data;



/* Module initialization fucntion */
static int __init gpio_init(void)
{
	/* 1. Create device class under /sys/class/ */
	gpiodrv_private_data.class_gpio = class_create(THIS_MODULE, "bone_gpios");
	/* 1.e Error handling of this section */
	if(IS_ERR(gpiodrv_private_data.class_gpio))
	{
		/* Print some error info */
		pr_err("Class creation failed\n");
		/*Convert pointer to error code int and return */
		return PTR_ERR(gpiodrv_private_data.class_gpio);
	}

	/* 2. Register a platform Driver in sysfs*/
	platform_driver_register(&gpio_platform_driver);

	/* Confirmation of successfully ended initialization */
	pr_info("Module  was loaded\n");

	return 0;
}
static void __exit gpio_cleanup(void)
{
	/* 1. Unregister platform driver in sysfs */
	platform_driver_unregister(&gpio_platform_driver);

	/* 2. Class destroying */
	class_destroy(gpiodrv_private_data.class_gpio);

  /* Some clean-up message */
  pr_info("Module was succesfully unloaded\n");
}

/* Adding function to init call */
module_init(gpio_init);

/* Adding function to exit call */
module_exit(gpio_cleanup);

/* Module description */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MKI");
MODULE_DESCRIPTION("A pseudo platform character device driver");