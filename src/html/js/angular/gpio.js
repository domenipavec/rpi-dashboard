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

registerPage('/gpio', {
    templateUrl: 'partials/gpio.html',
    controller: 'GpioController'
}, ['gpio'], "GPIO");

GpioPinClass = function(key, value) {
    this.id = key;
    this.mode = value.mode;
    this.pull = value.pull;
    this.pulls = ["off", "down", "up"];
    this.value = value.value;
    this.range = value.range;
    this.frequency = value.frequency;
};

GpioPinClass.prototype.getModes = function() {
    var modes = ["input", "output", "pwm", "tone"];
    if (this.mode == "undefined") {
        modes.unshift("undefined");
    }
    return modes;
};

GpioPinClass.prototype.modeChange = function() {
    $.rpijs.post("gpio/"+this.id+"/mode", this.mode);
};

GpioPinClass.prototype.pullChange = function() {
    $.rpijs.post("gpio/"+this.id+"/pull", this.pull);
};

GpioPinClass.prototype.pullShow = function() {
    return this.mode == "input";
};

GpioPinClass.prototype.valueChange = function() {
    console.log(this.value);
    if (this.value === null) {
        return;
    }
    if (typeof this.value != "number") {
        this.value = 0;
    }
    if (this.mode != "input") {
        $.rpijs.post("gpio/"+this.id+"/value", this.value);
    }
};

GpioPinClass.prototype.valueReadonly = function() {
    return this.mode == "input" || this.mode == "undefined";
};

GpioPinClass.prototype.rangeChange = function() {
    if (this.range === null) {
        return;
    }
    if (typeof this.range != "number") {
        this.range = 100;
    }
    $.rpijs.post("gpio/"+this.id+"/range", this.range);
    if (this.value > this.range) {
        this.value = this.range;
    }
};

GpioPinClass.prototype.rangeShow = function() {
    return this.mode == "pwm";
};

GpioPinClass.prototype.frequencyChange = function() {
    if (this.frequency === null) {
        return;
    }
    if (typeof this.frequency != "number") {
        this.frequency = 1000;
    }
    $.rpijs.post("gpio/"+this.id+"/frequency", this.frequency);
};

GpioPinClass.prototype.frequencyShow = function() {
    if (this.mode == "pwm" && this.id == "1") {
        return true;
    }
    return this.mode == "tone";
};

GpioPinClass.prototype.frequencyMax = function() {
    if (this.mode == "pwm") {
        return 19200000;
    } else {
        return 5000;
    }
};

GpioPinClass.prototype.toggleShow = function() {
    return this.mode == "output";
};

GpioPinClass.prototype.toggleClick = function() {
    if (this.value > 0) {
        this.value = 0;
    } else {
        this.value = 1;
    }
    this.valueChange();
};

rpiDashboard.controller("GpioController", function($scope) {
    $.rpijs.get("gpio", function(data) {
        $scope.$apply(function() {
            $scope.pins = [];
            angular.forEach(data, function (value, key) {
                $scope.pins.push(new GpioPinClass(key, value));
            });
        });
    });
});