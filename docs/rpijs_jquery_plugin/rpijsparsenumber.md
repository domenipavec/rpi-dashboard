# $.rpijs.parseNumber(value, [formatOptions])

Formats the number value according to formatOptions.

## Arguments

### value

Value to be formatted. If it is anything but a "number" type, it is just returned.

### formatOptions

Optional [formatOptions](formatoptions.md) object, to specify how the number should be formatted.

## Example
```javascript
var size = $.rpijs.parseNumber(item.size, {valueType: "binary"});
```
