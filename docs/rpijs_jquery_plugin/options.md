# options

Options object specifies options to be used when making a request to REST api.

The argument to the function extends the defaults:
```javascript
    $.rpijs.defaults = {
        update: 0,
        rate: false,
        format: [{}]
    };

```

You can change the default options by changing the **$.rpijs.defaults**.

## Available options

### update

Number of miliseconds between requests for updating. When 0, only the value will not be updated.

### rate

Specifies whether to store the previous request, to enable rate calculation in [$.rpijs.parse](rpijsparse.md).

### format

A list of [formatOptions](formatoptions.md), that specify how the return values will be formatted. If the return value is not an object, only the first entry will be used.
