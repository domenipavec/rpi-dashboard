(function ($, undefined) {
    
    $.rpijs = {};
    
    $.rpijs.rates = {};
    
    $.rpijs.defaults = {
        update: 0,
        rate: false,
        format: [{}]
    };
    
    $.rpijs.formatDefaults = {
        key: [],
        rate: false,
        valueType: "none",
        decimals: 2
    };
    
    /* initialize REST api details */
    $.rpijs.init = function(apiUrl, username, password) {
        $.rpijs.apiUrl = apiUrl;
        $.rpijs.username = username;
        $.rpijs.password = password;
    };
    
    /* get value according to specified options */
    $.rpijs.get = function(name, callback, options) {
        var settings = $.extend({}, $.rpijs.defaults, options);
        
        if (settings.rate) {
            $.rpijs.getRate(name, callback, options);
            return;
        }
        
        $.ajax({
            url: $.rpijs.apiUrl + name,
            headers: {
                Authorization: "Basic " + btoa($.rpijs.username + ":" + $.rpijs.password)
            }
        }).done(function(object) {
            var ret = callback($.rpijs.parse(object, name, options));
            
            if (ret && settings.update != 0) {
                setTimeout(function() {
                    $.rpijs.get(name, callback, options);
                }, settings.update);
            }
        });
    };
    
    /* Format all values specified in format option */
    $.rpijs.parse = function(objectArg, name, options) {
        var settings = $.extend({}, $.rpijs.defaults, options);
        
        /* if only one value */
        if (typeof objectArg !== "object") {
            var formatSettings = $.extend({}, $.rpijs.formatDefaults, settings.format[0]);
            
            /* calculate rate */
            if (typeof objectArg === "number" && formatSettings.rate) {
                objectArg = (objectArg - $.rpijs.rates[name].object)/($.now() - $.rpijs.rates[name].time)*1000;
            }
            
            return $.rpijs.parseNumber(objectArg, settings.format[0]);
        }

        /* make a copy of object for modification */
        var object = $.extend(true, {}, objectArg);
        
        $.each(settings.format, function(index, formatOptions) {
            var formatSettings = $.extend({}, $.rpijs.formatDefaults, formatOptions);
            
            /* get object owner of value and key */
            var value = object;
            var i;
            for (i = 0; i < formatSettings.key.length - 1; i++) {
                value = value[formatSettings.key[i]];
            }
            var key = formatSettings.key[i];
            
            /* calculate rate */
            if (typeof value[key] === "number" && formatSettings.rate) {
                var oldValue = $.rpijs.rates[name].object;
                $.each(formatSettings.key, function(index, key) {
                    oldValue = oldValue[key];
                });
                
                value[key] = (value[key] - oldValue)/($.now() - $.rpijs.rates[name].time)*1000;
            }
            
            value[key] = $.rpijs.parseNumber(value[key], formatOptions);
        });
        
        return object;
    };
    
    /* format the number according to format options */
    $.rpijs.parseNumber = function(value, formatOptions) {
        var formatSettings = $.extend({}, $.rpijs.formatDefaults, formatOptions);

        if (typeof value !== "number" || formatSettings.valueType === "none") {
            return value;
        } else {
            var divider, units;
            if (formatSettings.valueType == "decimal") {
                divider = 1000;
                units = ['B', 'kB','MB','GB','TB','PB','EB','ZB','YB'];
            } else if (formatSettings.valueType == "binary") {
                divider = 1024;
                units = ['B', 'KiB','MiB','GiB','TiB','PiB','EiB','ZiB','YiB'];
            } else if (formatSettings.valueType == "time") {
                var seconds = value;
                var ret = "";
                if (seconds > 60) {
                    var minutes = Math.floor(seconds/60);
                    seconds = seconds % 60;
                    if (minutes > 60) {
                        var hours = Math.floor(minutes/60);
                        minutes = minutes % 60;
                        if (hours > 24) {
                            ret += Math.floor(hours/24) + " days ";
                            hours = hours % 24;
                        }
                        ret += hours + " hours ";
                    }
                    ret += minutes + " min ";
                }
                ret += seconds.toFixed(formatSettings.decimals) + " s";
                return ret;
            } else {
                return value.toFixed(formatSettings.decimals);
            }
            var ps = "";
            if (formatSettings.rate) {
                ps = "/s";
            }
            var i = 0;
            while (value >= divider) {
                value /= divider;
                i++;
            }
            return value.toFixed(formatSettings.decimals) + " " + units[i] + ps;
        }
    };
    
    /* get values and store for rate calculation */
    $.rpijs.getRate = function(name, callback, options) {
        var settings = $.extend({}, $.rpijs.defaults, options);
        var getOptions = {
            update: 0,
            rate: false,
            format: [{
                key: [],
                rate: false,
                valueType: "none"
            }]
        };
        
        $.rpijs.get(name, function(object) {
            /* if we do not have previous value we make another request in 500 */
            if ($.rpijs.rates[name] === undefined) {
                setTimeout(function() {
                    $.rpijs.getRate(name,callback,options);
                }, 500);
            } else {
                var ret = callback($.rpijs.parse(object, name, options));
            
                /* only update if callback returns true */
                if (ret && settings.update != 0) {
                    setTimeout(function() {
                        $.rpijs.getRate(name,callback,options);
                    }, settings.update);
                }
            }
            /* store current value */
            $.rpijs.rates[name] = {
                object: object,
                time: $.now()
            };
        }, getOptions);
    };
    
    /* Stores value in jQuery html object */
    $.fn.rpijs = function(name, options) {
        return this.each(function() {
            var self = this;
            $.rpijs.get(name, function(value) {
                if (typeof value === "object") {
                    value = JSON.stringify(value);
                }
                $(self).html(value);
                return true;
            }, options);
        });
    };
    
}(jQuery));