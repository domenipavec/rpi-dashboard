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

registerPage('/shift', {
    templateUrl: 'partials/shift.html',
    controller: 'ShiftController'
}, ['shift'], "Shift", "Shift 8-bits of data in/out on data pin using another pin as a clock.");

rpiDashboard.controller("ShiftController", function($scope) {
    $scope.actions = [
        {
            id: 0,
            text: "Shift in"
        },
        {
            id: 1,
            text: "Shift out"
        }
    ];
    $scope.action = $scope.actions[0];
    $scope.order = "msbfirst";
    $scope.orders = ["msbfirst", "lsbfirst"];
    $scope.data = 1;
    $scope.clock = 0;
    $scope.value = 0;
    $scope.valueShow = function() {
        return $scope.action.id == 1;
    };
    $scope.finished = false;

    $scope.execute = function() {
        if ($scope.action.id == 0) {
            $.rpijs.post("shift/in", {order: $scope.order, data: $scope.data, clock: $scope.clock}, function(data) {
                if (typeof data != "number") {
                    $scope.status = "warning";
                    $scope.statusText = data;
                } else {
                    $scope.status = "success";
                    $scope.statusText = "Value: "+data;
                }
                $scope.finished = true;
            });
        } else {
            $.rpijs.post("shift/out", {order: $scope.order, data: $scope.data, clock: $scope.data, value: $scope.value}, function(data) {
                if (data != "Successful!") {
                    $scope.status = "warning";
                } else {
                    $scope.status = "success";
                }
                $scope.statusText = data;
                $scope.finished = true;
            });
        }
    };
    
    $scope.buttonClick = function() {
        $scope.finished = false;
    };
});