/*  Raspberry Pi Dasboard
 *  =====================
 *  Copyright 2014 Domen Ipavec <domen.ipavec@z-v.si>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/* start fragment */
/* Source: https://gist.github.com/chicagoworks/754454 */
jQuery.extend({
    stringify  : function stringify(obj) {         
        if ("JSON" in window) {
            return JSON.stringify(obj);
        }

        var t = typeof (obj);
        if (t != "object" || obj === null) {
            // simple data type
            if (t == "string") obj = '"' + obj + '"';

            return String(obj);
        } else {
            // recurse array or object
            var n, v, json = [], arr = (obj && obj.constructor == Array);

            for (n in obj) {
                v = obj[n];
                t = typeof(v);
                if (obj.hasOwnProperty(n)) {
                    if (t == "string") {
                        v = '"' + v + '"';
                    } else if (t == "object" && v !== null){
                        v = jQuery.stringify(v);
                    }

                    json.push((arr ? "" : '"' + n + '":') + String(v));
                }
            }

            return (arr ? "[" : "{") + String(json) + (arr ? "]" : "}");
        }
    }
});
/* end fragment */

// let's invite Firefox to the party.
if (window.MozWebSocket) {
    window.WebSocket = window.MozWebSocket;
}

(function ($, undefined) {
    
    $.rpijs = {};
    
    $.rpijs.rates = {};
    
    $.rpijs.defaults = {
        update: 0,
        rate: false,
        format: []
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
    
    /* return authorization string */
    var authString = function() {
        return "Basic " + btoa($.rpijs.username + ":" + $.rpijs.password);
    };
    
    /* get value according to specified options */
    $.rpijs.get = function(name, callback, options) {
        var settings = $.extend({}, $.rpijs.defaults, options);
        
        if (settings.rate) {
            getRate(name, callback, options);
            return;
        }
        
        $.ajax({
            url: $.rpijs.apiUrl + name,
            headers: {
                Authorization: authString()
            }
        }).done(function(object) {
            var ret = callback(parse(object, name, options));
            
            if (ret && settings.update != 0) {
                setTimeout(function() {
                    $.rpijs.get(name, callback, options);
                }, settings.update);
            }
        });
    };
    
    /* POST request to the REST api */
    $.rpijs.post = function(name, data, callback) {
        return $.ajax({
            url: $.rpijs.apiUrl + name,
            headers: {
                Authorization: authString()
            },
            type: "POST",
            data: $.stringify(data),
            contentType: "application/json"
        }).done(callback);
    };
    
    /* initialize WebSocket connection */
    $.rpijs.websocket = function(name, callback) {
        if (window.WebSocket === undefined) {
            return undefined;
        }
        var path = "";
        if (location.protocol == "https:") {
            path += "wss://";
        } else {
            path += "ws://";
        }
        path += location.host + location.pathname + $.rpijs.apiUrl + name;
        document.cookie = "RPiAuthorization="+authString();
        var conn = new WebSocket(path);
        if (callback !== undefined) {
            conn.onmessage = function(event) {
                callback($.parseJSON(event.data));
            };
        }
        return conn;
    };
    
    /* Format all values specified in format option */
    var parse = function(objectArg, name, options) {
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
    var getRate = function(name, callback, options) {
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
                    getRate(name,callback,options);
                }, 500);
            } else {
                var ret = callback(parse(object, name, options));
            
                /* only update if callback returns true */
                if (ret && settings.update != 0) {
                    setTimeout(function() {
                        getRate(name,callback,options);
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