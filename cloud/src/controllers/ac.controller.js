var Client = require('azure-iothub').Client;
var Registry = require('azure-iothub').Registry;
var Ac = require("../models/ac.model");

var connectionString = process.env.AZURE_CONNECTION_STRING;

var client = Client.fromConnectionString(connectionString);

var registry = Registry.fromConnectionString(connectionString);

exports.getAcSettings = (req, res) => {
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
                var ac = new Ac(twin.deviceId, twin.properties.desired.status, twin.properties.desired.temperature, twin.properties.desired.quite, twin.properties.desired.powerful, twin.properties.desired.fan);
                acSettings.push(ac.fields);
            });
            res.send(acSettings);
        }).catch(err => {
            res.status(500).send({
                message: err.message || "Some error occurred while retrieving Device Data."
            });
        });
};

exports.sendAcSetting = (req, res) => {
    var err = SendAcSettingToAzure(req.query.deviceId, req.body.devicePower, req.body.targetTemp, req.body.quiteOn, req.body.powerfulOn, req.body.fanMode, req.body.swingVOn, req.body.swingHOn);
    if (err) {
        res.status(500).send({
            message: err.message || "Some error occurred while updating properties."
        });
    }
    else {
        res.status(200).send();
    }
};

exports.SendAcSettingToAzure = (deviceId, devicePower, targetTemp, quiteOn, powerfulOn, fanMode, swingVOn, swingHOn) => {
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
                            targetTemp: targetTemp,
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

