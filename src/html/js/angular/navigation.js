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

rpiDashboard.factory('Navigation', function($location, User) {
    var ads = {};
    var menu = [];
    
    return {
        register: function(path, accessDependencies, menuName, menuTitle) {
            ads[path] = accessDependencies;
            if (menuTitle == undefined) {
                menuTitle = "";
            }
            menu.push({
                name: menuName,
                title: menuTitle,
                path: path
            });
        },
        registerDependencies: function(path, accessDependencies) {
            ads[path] = accessDependencies;
        },
        getDependencies: function() {
            return ads[$location.path()];
        },
        getMenu: function() {
            var curMenu = [];
            angular.forEach(menu, function(entry) {
                if (User.checkDependencies(ads[entry.path])) {
                    curMenu.push(entry);
                }
            });
            return curMenu;
        }
    };
});

rpiDashboard.controller('NavigationController', function($scope, $location, User, Navigation) {
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
});