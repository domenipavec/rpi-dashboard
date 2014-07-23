# /api/

Returns a JSON object with two properties:

### user

The name of successfully logged in user. If no user is logged in, or username and password are incorrect, this is *null*.

### modules

A list of modules that the user has permission to access.

Example:
```
{
	"user":	"admin",
	"modules":	["memory", "cpu", "network", "storage", "gpio", "i2c", "spi", "serial", "logger"]
}
```
