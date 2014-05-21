# Configuration

Raspberry Pi Dashboard is configured using plain text files. The configuration files [schema](files_schema.md) is inherited from [Monkey HTTP Server](http://monkey-project.com/).

## Structure

The configuration files are located in the **conf/** directory and have the following structure:
```
conf/
    /modules.conf
    /rpi.conf
    /rpi.users
```

## Files and Directories

### [rpi.conf](rpiconf.md)
This is the main configuration file, it describes global configuration as well as default configuration for modules.

### [rpi.users](rpiusers.md)
This file is used to store username and passwords. Use the tool from Monkey's auth plugin to create it.

### [modules.conf](modulesconf.md)
This file is used for configuring each module separately. Here you can set user access permission and disable modules.
