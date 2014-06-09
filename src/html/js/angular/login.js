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

var hasOwnValue = function(object, val) {
    for(var prop in object) {
        if(object.hasOwnProperty(prop) && object[prop] === val) {
            return true;   
        }
    }
    return false;
}

rpiDashboard.factory('User', function($q, $cookies, $rootScope) {
    var modules = null;

    var userFactory = {
        loggedIn: false,
        login: function(username, password) {
            if (username == undefined) {
                username = "";
            }
            if (password == undefined) {
                password = "";
            }
            userFactory.loggedIn = (username != "");
            $.rpijs.init("./api/", username, password);
            $.rpijs.get("", function(msg) {
                modules = msg;
                $rootScope.$broadcast('USER_STATUS_CHANGED');
            });
        },
        logout: function() {
            $cookies.RPiUsername = "";
            $cookies.RPiPassword = "";
            return userFactory.login();
        },
        getModules: function() {
            return modules;
        },
        checkDependencies: function(dependencies) {
            var success = true;
            angular.forEach(dependencies, function(value) {
                if (!hasOwnValue(modules, value)) {
                    success = false;
                }
            });
            return success;
        }
    };
    
    userFactory.login($cookies.RPiUsername, $cookies.RPiPassword);
    
    return userFactory;
});

rpiDashboard.controller('LoginController', function($scope, User, $location, $cookies) {
    if (User.loggedIn) {
        $location.path("/");
    }
    
    $scope.rememberMe = true;
    
    $scope.login = function() {
        User.login($scope.username, $scope.password);
        
        if ($scope.rememberMe) {
            $cookies.RPiUsername = $scope.username;
            $cookies.RPiPassword = $scope.password;
        }
    };
});

rpiDashboard.config(function($routeProvider) {
    $routeProvider.when('/login', {
        templateUrl: 'partials/login.html',
        controller: 'LoginController'
    });
});

rpiDashboard.run(function ($rootScope, $location, Navigation, User) {
    $rootScope.$on('$routeChangeStart', function (event) {
        if (User.getModules() === null) {
            return;
        }
        
        if (!User.checkDependencies(Navigation.getDependencies())) {
            event.preventDefault();
            $location.path('/login');
        }
    });
    
    $rootScope.$on('USER_STATUS_CHANGED', function() {
        if (User.loggedIn && $location.path() == "/login") {
            $location.path("/");
        }
        if (!User.checkDependencies(Navigation.getDependencies())) {
            $location.path('/login');
        }

    });
});
