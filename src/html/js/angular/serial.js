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

registerPage('/serial', {
    templateUrl: 'partials/serial.html',
    controller: 'SerialController'
}, ['serial'], "Serial", "Send and receive data through serial (UART) port.");

rpiDashboard.controller("SerialController", function($scope) {
    $scope.active = false;
    $scope.read = "";
    $scope.write = "";
    
    $scope.start = function() {
        $scope.active = true;
        $.rpijs.get("serial/port", function(data) {
            $scope.read += data;
            return $scope.active;
        }, {update:1000});
    };
    
    $scope.send = function() {
        if (!$scope.active) {
            $scope.start();
        }
        $.rpijs.post("serial/port", $scope.write);
        $scope.write = "";
    };
    
    $scope.$on("$destroy", function() {
        $scope.active = false;
    });
});