# $.rpijs.websocket(name, [callback])

Initialize WebSocket connection to *name* with correct RPiAuthorization cookie. Cookie is used because javascript WebSocket does not send Authorization header.

## Arguments

### name

Path under the REST api to connect to.

### callback

Function that will be called when message is received. Message will be parsed using jQuery's $.parseJSON.

## Return value

Javascript WebSocket object is returned. You can use it to register addidtional callbackd, for example *onopen* and *onclose*.

## Example
```javascript
$.rpijs.websocket("gpio/ws", function(data) {
    $.each(data, function (key, value) {
        var i = parseInt(key);
        pins[i].value = value;
    });
});
```
