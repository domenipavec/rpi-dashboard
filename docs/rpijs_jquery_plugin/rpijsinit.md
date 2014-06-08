# $.rpijs.init(apiUrl, username, password)

Init function needs to be called before any access to the REST api is made. It initializes the required parameters for accessing the REST api.

## Arguments

### apiUrl

Path to the base of the REST api. It must include the trailing slash. This can be a relative or absolute URL.

Example:
```
"http://localhost:2001/rpi/api/"
```

### username

Username to be used in HTTP auth, when accessing REST api.

### password

Password to be used in HTTP auth, when accessing REST api.
