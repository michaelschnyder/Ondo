var Registry = require('azure-iothub').Registry;

var connectionString = process.env.AZURE_CONNECTION_STRING;

var registry = Registry.fromConnectionString(connectionString);

exports.readSensorData = (req, res) => {
    var query;
    if (req.query.deviceId == undefined) {
        query = registry.createQuery("SELECT * FROM devices", 100);
    } else {
        query = registry.createQuery("SELECT * FROM devices WHERE deviceId = '" + req.query.deviceId + "'", 100);
    }
    query.nextAsTwin()
        .then(q => {
            var roomCondition = new Array();
            q.result.forEach(function (twin) {
                var sensor = {
                    deviceId: twin.deviceId,
                    currentTempC: twin.properties.reported.currentTempC,
                    currentHumidity: twin.properties.reported.currentHumidity
                }
                roomCondition.push(sensor);
            });

            res.send(roomCondition);
        }).catch(err => {
            res.status(500).send({
                message: err.message || "Some error occurred while retrieving Entry."
            });
        });
};

exports.getTwins = (req, res) => {
    var query = registry.createQuery('SELECT * FROM devices', 100);
    query.nextAsTwin()
        .then(q => {
            var devices = new Array();
            q.result.forEach(function (twin) {
                //TODO: Add Location
                var device = { deviceId: twin.deviceId }
                devices.push(device.fields);
            });
            res.send(devices);
        }).catch(err => {
            res.status(500).send({
                message: err.message || "Some error occurred while retrieving Entry."
            });
        });
}