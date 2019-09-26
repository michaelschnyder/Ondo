var Client = require('azure-iothub').Client;
var Registry = require('azure-iothub').Registry;
var ac_data = require('../data/ac');

var connectionString = process.env.AZURE_CONNECTION_STRING;

var client = Client.fromConnectionString(connectionString);

var registry = Registry.fromConnectionString(connectionString);

var getAcSettings = function (req, res) {
    var query;
    if (req.query.deviceId == undefined) {
        query = registry.createQuery("SELECT * FROM devices", 100);
    } else {
        query = registry.createQuery("SELECT * FROM devices WHERE deviceId = '" + req.query.deviceId + "'", 100);
    }
    query.nextAsTwin()
        .then(q => {
            var acSettings = new Array();
            q.result.forEach(function (twin) {
                var ac = {
                    "deviceId": twin.deviceId,
                    "devicePower": twin.properties.desired.devicePower,
                    "targetTempC": twin.properties.desired.targetTempC,
                    "fanMode": twin.properties.desired.fanMode,
                    "powerfulOn": twin.properties.desired.powerfulOn,
                    "quiteOn": twin.properties.desired.quiteOn,
                    "swingVOn": twin.properties.desired.swingVOn,
                    "swingHOn": twin.properties.desired.swingHOn
                };
                acSettings.push(ac);
            });
            res.send(acSettings);
        }).catch(err => {
            res.status(500).send({
                message: err.message || "Some error occurred while retrieving Device Data."
            });
        });
};

var sendAcSetting = function (req, res) {
    var err = SendAcSettingToAzure(req.query.deviceId, req.body.devicePower, req.body.targetTempC, req.body.quiteOn, req.body.powerfulOn, req.body.fanMode, req.body.swingVOn, req.body.swingHOn);
    if (err) {
        res.status(500).send({
            message: err.message || "Some error occurred while updating properties."
        });
    }
    else {
        res.status(200).send();
    }
};

var turnOff = function(req, res){
    console.log("turnOff");
    TurnOffAc(ac_data.MASTER_ROOM);
    TurnOffAc(ac_data.DINING_ROOM);
    TurnOffAc(ac_data.GUEST_ROOM);
    TurnOffAc(ac_data.LIVING_ROOM);
}

var turnOn = function(req, res){
    TurnOnAc(ac_data.LIVING_ROOM);
}

var SendAcSettingToAzure = function (deviceId, devicePower, targetTempC, quiteOn, powerfulOn, fanMode, swingVOn, swingHOn) {
    client.open(function (err) {
        if (err) {
            console.error('could not open IotHub client');
            return err;
        }
        else {
            console.log('client opened');
            registry.getTwin(deviceId, async (err, twin) => {
                if (err) {
                    return err;
                }
                else {
                    console.log('Got device twin');
                }
                var twinPatchAc = {
                    properties: {
                        desired: {
                            devicePower: devicePower,
                            targetTempC: targetTempC,
                            quiteOn: quiteOn,
                            powerfulOn: powerfulOn,
                            fanMode: fanMode,
                            swingVOn: swingVOn,
                            swingHOn: swingHOn,
                        }
                    }
                };
                twin.update(twinPatchAc, function (err) {
                    return err;
                });
            });
        }
    });
}

exports.TurnOnAc = (deviceId) => {
    console.log(`Device with DeviceId: ${deviceId}, Turn On Ac`);
    var devicePower = 1;
    var targetTempC = 24;
    var quiteOn = true;
    var powerfulOn = false;
    var fanMode = 1;
    var swingVOn = false;
    var swingHOn = false;
    SendAcSettingToAzure(deviceId, devicePower, targetTempC, quiteOn, powerfulOn, fanMode, swingVOn, swingHOn);
}

exports.TurnOffAc = (deviceId) => {
    console.log(`Device with DeviceId: ${deviceId}, Turn Off Ac`)
    var devicePower = 0;
    var targetTempC = 24;
    var quiteOn = true;
    var powerfulOn = false;
    var fanMode = 1;
    var swingVOn = false;
    var swingHOn = false;
    SendAcSettingToAzure(deviceId, devicePower, targetTempC, quiteOn, powerfulOn, fanMode, swingVOn, swingHOn);
}

module.exports = {
    SendAcSettingToAzure: SendAcSettingToAzure,
    sendAcSetting: sendAcSetting,
    getAcSettings: getAcSettings,
    turnOff: turnOff,
    turnOn: turnOn
}

