# $.rpijs.post(name, data, callback)

Make a POST request to the REST api. Used for all operations that need to send some data to server.

## Arguments

### name

Path under the REST api, that you want to request.

### data

Data to be sent to server. Use javascript object format, it will be transformed to JSON using *JSON.stringify*.

### callback

Function that will be called when the request has been returned.

The function should take one argument, that is the returned content.

## Return value

Returns $.ajax object, that can be used for error handling.

## Example

```javascript
$.rpijs.post("gpio/0", {
        mode: "output",
        value: 1
    }, function(object) {
        console.log(object);
    });
```
