# $.fn.rpijs(name, [options])

This is a wrapper around the [$.rpijs.get](rpijsget.md) function. It stores the returned value in a jQuery DOM object.

## Arguments

### name

Path under the REST api, that you want to request.

### [options]
Optional [Options](options.md) object specifies all other request options.


## Example
```javascript
$("#memory-used").rpijs("memory/used", {update: 5000});
```
