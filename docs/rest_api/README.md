# REST api

Raspberry Pi Dashboard access all hardware features and values through the REST api, that is written using DUDA I/O. REST api provides access to all values in json format.

The main URL is /rpi/api/, it lists the available modules for the user, who is logged in using HTTP auth.

All modules are hierarchical, which means that all values can be accessed through /rpi/api/module_name, which returns json object structure of values. Alternatively you can access sub-objects or values with a more detailed url.

Access to REST api is with GET method, unless otherwise specified.
