var CronJob = require('cron').CronJob;
var acController = require('../controllers/ac.controller');
var ac = require('../data/ac');

//Everyday at 10pm
var ruleEverydayAt10pm = '00 00 22 * * *';

//Everyday at 10am
var ruleWeekendsAt11am = '00 00 11 * * 5-6';

//Everyday at 2am
var ruleEverydayAt2am = '00 00 2 * * *';

//Everyday at 6am
var ruleEverydayAt6am = '00 00 6 * * *';

//Workdays at 9am
var ruleWorkdayAt9am = '00 00 9 * * 0-4'

//Workdays at 5pm
var ruleWorkdayAt5pm = '00 00 17 * * 0-4'

var timeZone = "Asia/Singapore";

exports.scheduledJobs = () => {
    
    //MasterRoom
    new CronJob(ruleEverydayAt10pm, function(){ acController.TurnOnAc(ac.MASTER_ROOM.deviceId)}, null, true, timeZone);
    new CronJob(ruleWeekendsAt11am, function(){ acController.TurnOffAc(ac.MASTER_ROOM.deviceId)}, null, true, timeZone);
    new CronJob(ruleWorkdayAt9am, function(){ acController.TurnOffAc(ac.MASTER_ROOM.deviceId)}, null, true, timeZone);

    //GuestRoom
    new CronJob(ruleEverydayAt10pm, function(){ acController.TurnOffAc(ac.GUEST_ROOM.deviceId)}, null, true, timeZone);
    new CronJob(ruleWorkdayAt9am, function(){ acController.TurnOffAc(ac.GUEST_ROOM.deviceId)}, null, true, timeZone);

    //DiningRoom
    new CronJob(ruleEverydayAt10pm, function(){ acController.TurnOffAc(ac.DINING_ROOM.deviceId)}, null, true, timeZone);
    new CronJob(ruleWorkdayAt9am, function(){ acController.TurnOffAc(ac.DINING_ROOM.deviceId)}, null, true, timeZone);

    //LivingRoom 
    new CronJob(ruleEverydayAt2am, function(){ acController.TurnOffAc(ac.LIVING_ROOM.deviceId)}, null, true, timeZone);
    new CronJob(ruleEverydayAt6am, function(){ acController.TurnOnAc(ac.LIVING_ROOM.deviceId)}, null, true, timeZone);
    new CronJob(ruleWorkdayAt9am, function(){ acController.TurnOffAc(ac.LIVING_ROOM.deviceId)}, null, true, timeZone);
    new CronJob(ruleWorkdayAt5pm, function(){ acController.TurnOnAc(ac.LIVING_ROOM.deviceId)}, null, true, timeZone);
}
