(function ($, undefined) {
    
    $.rpijs = {};
    
    $.rpijs.rates = {};
    
    $.rpijs.defaults = {
        update: 0,
        rate: false,
        format: "none",
        decimals: 1
    }
    
    $.rpijs.init = function(apiUrl, username, password) {
        $.rpijs.apiUrl = apiUrl;
        $.rpijs.username = username;
        $.rpijs.password = password;
    }
        
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
        }).done(function(value) {
            callback($.rpijs.parseNumber(value, options));
            
            if (settings.update != 0) {
                setTimeout(function() {
                    $.rpijs.get(name, callback, options);
                }, settings.update);
            }
        });
    }
    
    $.rpijs.parseNumber = function(value, options) {
        var settings = $.extend({}, $.rpijs.defaults, options);

        if (typeof value !== "number" || settings.format === "none") {
            return value;
        } else {
            var divider, units;
            if (settings.format == "decimal") {
                divider = 1000;
                units = ['B', 'kB','MB','GB','TB','PB','EB','ZB','YB'];
            } else if (settings.format == "binary") {
                divider = 1024;
                units = ['B', 'KiB','MiB','GiB','TiB','PiB','EiB','ZiB','YiB'];
            }else {
                return value.toFixed(settings.decimals);
            }
            var ps = "";
            if (settings.rate) {
                ps = "/s";
            }
            var i = 0;
            while (value >= divider) {
                value /= divider;
                i++;
            }
            return value.toFixed(settings.decimals) + " " + units[i] + ps;
        }
    }
    
    $.rpijs.getRate = function(name, callback, options) {
        var settings = $.extend({}, $.rpijs.defaults, options);
        var getOptions = {
            update: 0,
            rate: false,
            format: "none"
        };
        
        $.rpijs.get(name, function(value) {
            if (typeof value === "number") {
                var t = $.now();
                if ($.rpijs.rates[name] === undefined) {
                    setTimeout(function() {
                        $.rpijs.getRate(name,callback,options);
                    }, 500);
                } else {
                    var rate = (value - $.rpijs.rates[name].value)/(t - $.rpijs.rates[name].time)*1000;
                    callback($.rpijs.parseNumber(rate, options));
                
                    if (settings.update != 0) {
                        setTimeout(function() {
                            $.rpijs.getRate(name,callback,options);
                        }, settings.update);
                    }
                }
                $.rpijs.rates[name] = {
                    value: value,
                    time: t
                };

            } else {
                callback(NaN);
            }
        }, getOptions);
    }
    
    $.fn.rpijs = function(name, options) {
        return this.each(function() {
            var self = this;
            $.rpijs.get(name, function(value) {
                $(self).html(value);
            }, options);
        });
    }
    
}(jQuery));