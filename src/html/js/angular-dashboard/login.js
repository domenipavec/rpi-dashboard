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

/* factory to handle user login and logout */
rpiDashboard.factory('User', ['$q', '$cookies', '$rootScope', '$location', function($q, $cookies, $rootScope, $location) {
    /* modules user has access to */
    var modules = [];
    var modules_write = [];

    var userFactory = {
        loggedIn: false,
        username: null,
        goNext: "/",
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
                modules_write = msg['modules-write'];
                if (userFactory.loggedIn || username == "") {
                    if (remember) {
                        $cookies.RPiUsername = username;
                        $cookies.RPiPassword = password;
                    }
                    if ($location.path() == "/login") {
                        $location.path(userFactory.goNext);
                    }
                } else {
                    $rootScope.$apply(function() {
                        $rootScope.msgError = "Invalid username or password.";
                    });
                }
                $rootScope.$broadcast('USER_STATUS_CHANGED');
            });
        },
        logout: function() {
            $cookies.RPiUsername = "";
            $cookies.RPiPassword = "";
            $rootScope.msgError = "";
            modules = [];
            return userFactory.login();
        },
        getModules: function() {
            return modules;
        },
        /* accepts list of read dependencies, or dict of read write deps */
        checkDependencies: function(dependencies) {
            var success = true;
            if (angular.isArray(dependencies)) {
                angular.forEach(dependencies, function(value, key) {
                    if ($.inArray(value, modules) == -1) {
                        success = false;
                    }
                });
            } else {
                angular.forEach(dependencies, function(value, key) {
                    if (value == 'write') {
                        if ($.inArray(key, modules_write) == -1 || $.inArray(key, modules) == -1) {
                            success = false;
                        }
                    } else {
                        if ($.inArray(key, modules) == -1) {
                            success = false;
                        }
                    }
                });
            }
            return success;
        }
    };
    
    userFactory.login($cookies.RPiUsername, $cookies.RPiPassword);
    
    return userFactory;
}]);

/* controller for login page */
rpiDashboard.controller('LoginController', ['$scope', 'User', '$location', '$cookies', function($scope, User, $location, $cookies) {    
    $scope.rememberMe = true;
    $scope.loggedIn = User.loggedIn;
    $scope.user = User.username;
    
    $scope.login = function() {
        User.login($scope.username, $scope.password, $scope.rememberMe);
    };
    
    $scope.logout = function() {
        User.logout();
    };
}]);

/* register login page */
rpiDashboard.config(['$routeProvider', function($routeProvider) {
    $routeProvider.when('/login', {
        templateUrl: 'partials/login.html',
        controller: 'LoginController'
    });
}]);

rpiDashboard.run(['$rootScope', '$location', 'Navigation', 'User', function ($rootScope, $location, Navigation, User) {
    /* check permission on route change */
    $rootScope.$on('$routeChangeStart', function (event, prev,  curr) {
        if (!User.checkDependencies(Navigation.getDependencies())) {
            if (User.loggedIn) {
                $rootScope.msgError = "You do not have permission to access '"+$location.path()+"'.";
            }
            event.preventDefault();
            if (prev !== undefined) {
                User.goNext = prev.$$route.originalPath;
            }
            $location.path('/login');
        }
    });
    
    /* check permission on login/logout */
    $rootScope.$on('USER_STATUS_CHANGED', function() {
        if (!User.checkDependencies(Navigation.getDependencies())) {
            User.goNext = $location.path();
            $location.path('/login');
            $rootScope.$apply();
        }
    });
}]);
