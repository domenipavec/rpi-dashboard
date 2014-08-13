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
    var ws = undefined;
    
    $scope.baud = 9600;
    $scope.baudOptions = [50, 110, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200];
    $.rpijs.get("serial/baud", function(msg) {
        $scope.baud = msg;
    });
    $scope.baudChange = function() {
        $.rpijs.post("serial/baud", $scope.baud);
    };
    
    var poll = function() {
        $.rpijs.get("serial/port", function(data) {
            $scope.$apply(function() {
                $scope.read += data;
            });
            return $scope.active;
        }, {update:1000});
    };
    
    $scope.start = function() {
        $scope.active = true;
        ws = $.rpijs.websocket("serial/ws", function(data) {
            $scope.$apply(function() {
                $scope.read += data;
            });
        });
        if (ws !== undefined) {
            ws.onclose = function() {
                poll();
            };
        } else {
            poll();
        }
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
        if (ws !== undefined) {
            ws.close();
        }
    });
});