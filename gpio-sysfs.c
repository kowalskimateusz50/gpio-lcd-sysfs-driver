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
#include <linux/of_device.h>
#include <linux/gpio/consumer.h>


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
  struct gpio_desc *desc;
};

/* Driver private data structure */
struct gpiodrv_private_data
{
  int total_devices;
  struct class *class_gpio;
};
/*Definition variable of private data structure*/
struct gpiodrv_private_data gpiodrv_private_data;

/* Attribute show and store methods for direction attribute */
ssize_t direction_show(struct device *dev, struct device_attribute *attr,char *buf)
{
  return 0;
}
ssize_t direction_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
  return 0;
}

/* Attribute show and store methods for value attribute */
ssize_t value_show(struct device *dev, struct device_attribute *attr,char *buf)
{
  return 0;
}
ssize_t value_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
  return 0;
}

/* Attribute show and store methods for label attribute */
ssize_t label_show(struct device *dev, struct device_attribute *attr,char *buf)
{
  return 0;
}

/*Definition variable of direction device attribute */
static DEVICE_ATTR_RW(direction);
static DEVICE_ATTR_RW(value);
static DEVICE_ATTR_RO(label);

/*Definition arrays of attributes */
static struct attribute *gpio_attrs[] =
{
  &dev_attr_direction.attr,
  &dev_attr_value.attr,
  &dev_attr_label.attr,
  NULL
};

/*Definition of group variable of attributes */
static struct attribute_group gpio_attr_group =
{
  .attrs = gpio_attrs
};

/*Definition of array attribute group */
static const struct attribute_group* gpio_attr_groups[] =
{
  &gpio_attr_group,
  NULL
};

/* Remove function of GPIO driver */
int gpio_remove(struct platform_device *gpio_dev)
{
  return 0;
}

/* Probe function of GPIO driver */
int gpio_probe(struct platform_device *gpio_dev)
{
  /*Structure of device */
  struct device *dev = &gpio_dev -> dev;

  /*Placeholder for name of property */
  const char *name;

  /*Definition of iterator */
  int i;

  /*Definition of error's return value */
  int ret;

  /* Parent device node variable definition */
  struct device_node *parent = dev -> of_node;

  /* Child device node variable definition */
  struct device_node *child = NULL;

  /* Structure of device private data */
  struct gpiodev_private_data *dev_data;

  /* Definition of structure for receive information from device_create_with_groups function */
  struct device *dev_sysfs;

  for_each_available_child_of_node(parent, child)
  {
    /* 1. Dynamically allocate memory for the device private data */
    dev_data = devm_kzalloc(dev, sizeof(*dev_data), GFP_KERNEL);
    if(!dev_data)
    {
      /* 1e. Dynamically allocate memory for the device private data */
      dev_err(dev, "Cannot allocate memory\n");
      return -ENOMEM;
    }
    /*1.f Extracting label data from device tree */
    if(of_property_read_string(child, "label", &name))
    {
      dev_warn(dev, "Missing label information\n");
      snprintf(dev_data -> label, sizeof(dev_data -> label), "unkngpio%d", i);
    }
    else
    {
      strcpy(dev_data-> label, name);
      dev_info(dev, "GPIO label = %s\n", dev_data -> label);
    }
  }

  /*2. Extract data from gpio */
  dev_data -> desc = devm_fwnode_get_gpiod_from_child(dev, "bone", &child-> fwnode, GPIOD_ASIS,  dev_data -> label);

  /*2.e Erorr handling */
  if(IS_ERR(dev_data -> desc))
  {
    ret = PTR_ERR(dev_data -> desc);
    if(ret == -ENOENT)
    {
      dev_err(dev,"No GPIO's has been assigned to the requested function and/or index\n");
    }
    return ret;
  }

  /*3. Set the gpio direction for output */
  ret = gpiod_direction_output(dev_data-> desc, 0);
  if(ret)
  {
    dev_err(dev, "Gpio direction set failed\n");
    return ret;
  }

  /*4. Create devices under /sys/class/bone_gpios */
  dev_sysfs = device_create_with_groups(gpiodrv_private_data.class_gpio, dev, 0, dev_data, gpio_attr_groups, dev_data -> label);

  /*4.e Error handling for device_create_with_groups function */
  if(IS_ERR(dev_sysfs))
  {
    dev_err(dev, "Error during device creation\n");
    return PTR_ERR(dev_sysfs);
  }
/*iteration number of devices */
  i++;

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
    .name = "bone-gpio-sysfs",
    .of_match_table = of_match_ptr(gpiodrv_dt_match)
  }

};

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
