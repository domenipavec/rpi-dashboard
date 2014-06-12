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

rpiDashboard = angular.module('rpiDashboard', ['ngRoute', 'ngCookies', 'googlechart']);

registerPage = function(path, options, accessDependencies, menuName) {
    rpiDashboard.config(function($routeProvider) {
        $routeProvider.when(path, options);
    });
    rpiDashboard.run(function(Navigation) {
        Navigation.register(path, accessDependencies, menuName);
    });
};

backgroundUpdate = function(dependencies, t, f) {
    rpiDashboard.run(function($rootScope, User, $timeout, $q) {
        var active = User.checkDependencies(dependencies);
        var update = function() {
            if (!active) {
                return;
            }
            var done = $q.defer();
            f(done);
            done.promise.then(function() {
                if (t != 0) {
                    $timeout(update, t);
                }
            });
        };

        $rootScope.$on('USER_STATUS_CHANGED', function() {
            active = User.checkDependencies(dependencies);
            update();
        });

        update();
    });
};

vObject = function(value, filter) {
    return {"v":value};
};

cObject = function(value) {
    return {"c":value};
};

rpiDashboard.config(function($routeProvider) {
    $routeProvider.otherwise({
        redirectTo: "/memory"
    });
});

var bytesFilter = function(value, precision) {
    return $.rpijs.parseNumber(value, {valueType: 'binary', decimals: precision});
};
rpiDashboard.filter('bytes', function() {
    return bytesFilter;
});

rpiDashboard.filter('procents', function() {
    return function(value, precision) {
        if (isNaN(parseFloat(value)) || !isFinite(value)) return '-';
        if (typeof precision === 'undefined') precision = 1;
        return (100*value).toFixed(precision) + "%";
    };
});

rpiDashboard.filter('time', function() {
    return function(value) {
        return $.rpijs.parseNumber(value, {valueType: 'time', decimals: 0});
    }
});