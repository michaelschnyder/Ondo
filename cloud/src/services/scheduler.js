var CronJob = require('cron').CronJob;
var acController = require('../controllers/ac.controller');
var ac = require('../data/ac');

//Everyday at 11pm
var ruleEverydayAt11pm = '00 00 23 * * *';

//Everyday at 10pm
var ruleEverydayAt10pm = '00 00 22 * * *';

//Everyday at 10am
var ruleEverydayAt11am = '00 00 11 * * *';

//Everyday at 2am
var ruleEverydayAt2am = '00 00 2 * * *';

//Everyday at 6am
var ruleEverydayAt6am = '00 00 6 * * *';

var timeZone = "Asia/Singapore";

exports.scheduledJobs = () => {
    //MasterRoom

    new CronJob(ruleEverydayAt10pm, function(){ acController.TurnOnAc(ac.MASTER_ROOM.deviceId)}, null, true, timeZone);
    new CronJob(ruleEverydayAt11am, function(){ acController.TurnOffAc(ac.MASTER_ROOM.deviceId)}, null, true, timeZone);

    //GuestRoom
    new CronJob(ruleEverydayAt11pm, function(){ acController.TurnOffAc(ac.GUEST_ROOM.deviceId)}, null, true, timeZone);

    //DiningRoom
    new CronJob(ruleEverydayAt11pm, function(){ acController.TurnOffAc(ac.DINING_ROOM.deviceId)}, null, true, timeZone);

    //LivingRoom 
    new CronJob(ruleEverydayAt2am, function(){ acController.TurnOffAc(ac.LIVING_ROOM.deviceId)}, null, true, timeZone);
    new CronJob(ruleEverydayAt6am, function(){ acController.TurnOnAc(ac.LIVING_ROOM.deviceId)}, null, true, timeZone);
}
