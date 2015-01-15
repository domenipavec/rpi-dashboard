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

rpiDashboard.factory('Navigation', ['$location', 'User', function($location, User) {
    var accessDependencies = {};
    var menu = {};
    
    return {
        register: function(path, ads, menuName, menuTitle) {
            accessDependencies[path] = ads;
            if (menuTitle == undefined) {
                menuTitle = "";
            }
            menu[path] = {
                name: menuName,
                title: menuTitle,
                path: path
            };
        },
        registerDependencies: function(path, ads) {
            accessDependencies[path] = ads;
        },
        getDependencies: function() {
            return accessDependencies[$location.path()];
        },
        getMenu: function() {
            var curMenu = [];
            angular.forEach(menu, function(entry) {
                if (User.checkDependencies(accessDependencies[entry.path])) {
                    curMenu.push(entry);
                }
            });
            return curMenu;
        },
        getMenuEntry: function(path) {
            return menu[path];
        }
    };
}]);

rpiDashboard.controller('NavigationController', ['$scope', '$location', 'User', 'Navigation', function($scope, $location, User, Navigation) {
    $scope.loggedIn = false;
    $scope.menu = [];
    
    $scope.$on('USER_STATUS_CHANGED', function() {
        $scope.loggedIn = User.loggedIn;
        $scope.menu = Navigation.getMenu();
    });
    
    $scope.menuClass = function(page) {
        var current = $location.path().substring(1);
        return page.substring(1) === current ? "active" : "";
    };
    
    $scope.logout = function() {
        User.logout();
    }
}]);

rpiDashboard.controller('TitleDescriptionController', ['$scope', '$location', 'Navigation', '$rootScope', function($scope, $location, Navigation, $rootScope) {
    $scope.description = "";
    $scope.title = "";
    var refresh = function() {
        var entry = Navigation.getMenuEntry($location.path());
        if (entry == undefined) {
            $scope.titleShow = false;
            $scope.descriptionShow = false;
        } else {
            if (entry.name == undefined) {
                $scope.titleShow = false;
            } else {
                $scope.titleShow = true;
                $scope.title = entry.name;
            }
            if (entry.title == undefined) {
                $scope.descriptionShow = false;
            } else {
                $scope.descriptionShow = true;
                $scope.description = entry.title;
            }
        }
    };
    refresh();

    $rootScope.$on('$routeChangeStart', function() {
        refresh();
    });
}]);
