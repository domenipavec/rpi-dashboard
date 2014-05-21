# modules.conf

This is a configuration file for all modules.

    [module_name]

Each module is configured in it's own section. Section name is the name of the module.

Each module supports the following options:

### Disabled

This option disables the module.

* **on**

  When set to **on**, the module is disabled.

* **off**

  When set to **off**, the module is enabled.


    Disabled off

By default the module is enabled.

### Access

Sets who has access to this module. Available options are:
* **guests**

  This option allows access to anyone who can reach the web service.

* **users**

  This option allows access to any user, who has a registered username and password in [rpi.users](rpiusers.md).

* **list**

  This option allows only users listed in [rpi.conf](rpiconf.md) **DefaultAllowedUsers** or overriden in **AllowedUsers**.

This option overrides **DefaultAccess** in [rpi.conf](rpiconf.md). When this option is not present, **DefaultAccess** is used.

    #Access list

By default this option is commented out.

### AllowedUsers

List users that have access to this module, when **DefaultAccess** in [rpi.conf](rpiconf.md) or overriden in **Access** is set to **list**. Multiple users can be set, separated with spaces.

This option overrides **DefaultAllowedUsers** in [rpi.conf](rpiconf.md). When this option is not present, **DefaultAllowedUsers** is used.

    #AllowedUsers admin

By default this option is commented out.
