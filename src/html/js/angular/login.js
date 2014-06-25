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

rpiDashboard.factory('User', function($q, $cookies, $rootScope, $location) {
    var modules = null;

    var userFactory = {
        loggedIn: false,
        username: null,
        login: function(username, password, remember) {
            $rootScope.msgError = "";
            if (username == undefined) {
                username = "";
            }
            if (password == undefined) {
                password = "";
            }
            $.rpijs.init("./api/", username, password);
            $.rpijs.get("", function(msg) {
                userFactory.loggedIn = msg.user !== null;
                userFactory.username = msg.user;
                modules = msg.modules;
                $rootScope.$broadcast('USER_STATUS_CHANGED');
                if (userFactory.loggedIn || username == "") {
                    if (remember) {
                        $cookies.RPiUsername = username;
                        $cookies.RPiPassword = password;
                    }
                    $location.path("/");
                } else {
                    $rootScope.$apply(function() {
                        $rootScope.msgError = "Invalid username or password.";
                    });
                }
            });
        },
        logout: function() {
            $cookies.RPiUsername = "";
            $cookies.RPiPassword = "";
            $rootScope.msgError = "";
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
    $scope.rememberMe = true;
    $scope.loggedIn = User.loggedIn;
    $scope.user = User.username;
    
    $scope.login = function() {
        User.login($scope.username, $scope.password, $scope.rememberMe);
    };
    
    $scope.logout = function() {
        User.logout();
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
            if (User.loggedIn) {
                $rootScope.msgError = "You do not have permission to access '"+$location.path()+"'.";
            }
            event.preventDefault();
            $location.path('/login');
        }
    });
    
    $rootScope.$on('USER_STATUS_CHANGED', function() {
        if (!User.checkDependencies(Navigation.getDependencies())) {
            $location.path('/login');
            $rootScope.$apply();
        }
    });
    
    $location.path('/login');
});
