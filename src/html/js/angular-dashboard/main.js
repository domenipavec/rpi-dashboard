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

rpiDashboard.config(['$routeProvider', function($routeProvider) {
    $routeProvider.when('/', {
        templateUrl: 'partials/main.html',
        controller: 'MainController'
    });
}]);
rpiDashboard.run(['Navigation', function(Navigation) {
    Navigation.registerDependencies('/', []);
}]);

mainData = {};
mainData.widgets = [];

registerWidget = function(width, controller, partial, accessDependencies) {
    mainData.widgets.push({
        width: width,
        controller: controller,
        partial: partial,
        access: accessDependencies
    });
};

rpiDashboard.controller('MainController', ['$scope', 'User', function($scope, User) {
    var filterWidgets = function() {
        var fw = [];
        angular.forEach(mainData.widgets, function(widget) {
            if (User.checkDependencies(widget.access)) {
                fw.push(widget); 
            }
        });
        $scope.widgets = fw;
        $( "#main-sort" ).sortable({
            handle: ".panel-heading",
            placeholder: "col-sm-3 main-placeholder"
        });
        $( "#main-sort" ).disableSelection();
    };
    
    $scope.$on("USER_STATUS_CHANGED", function() {
        $scope.loggedIn = User.loggedIn;
        filterWidgets();
    });
    
    $scope.loggedIn = User.loggedIn;
    filterWidgets();
}]);
