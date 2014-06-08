# $.rpijs.get(name, callback, [options])

Get function makes a request to the REST api.

## Arguments

### name

Path under the REST api, that you want to request.

### callback

Function that will be called when the request has been returned. The function should take one argument, that is the returned content.

### [options]
Optional [Options](options.md) object specifies all other request options.

## Example
```javascript
    $.rpijs.get("cpu/usage", function(msg) {
        $("#cpu-usage").html((msg.busy/msg.total*100).toFixed(1));
        return true;
    }, {
        update: 1000,
        rate: true,
        format: [{
            key: ["total"],
            rate: true,
            valueType: "none"
        },
        {
            key: ["busy"],
            rate: true,
            valueType: "none"
        }]
    });
```
