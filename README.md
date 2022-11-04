GPIO sysfs driver

The goal of project is to handle GPIOs of the hardware through sysfs interface

The driver should support the below functionality
1)The driver should create a class "bone_gpios" under /sys/class class_create


2)For every detected GPIO in the device tree, the driver should create a device under /sys/class/bone_gpios device_create


3)The driver should also create 3 sysfs files (attributes ) for every gpio device
attribute 1) direction: used to configure the gpio direction possible values: ‘in’ and ‘out’ mode : (read /write)
attribute 2) value: used to enquire the state of the gpio or to write a new value to the gpio possible values : 0 and 1 (read/write)
attribute 3) label: used to enquire label of the gpio (read only )


4)Handling of 16x2 1602A LCD Display
