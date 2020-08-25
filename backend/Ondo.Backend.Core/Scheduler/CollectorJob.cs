using System;
using System.Threading;
using Microsoft.Extensions.Logging;
using Ondo.Backend.Core.Services;
using Quartz;
using System.Threading.Tasks;

namespace Ondo.Backend.Core.Scheduler
{
    public class CollectorJob : IJob
    {
        private readonly ILogger<CollectorJob> _logger;
        private readonly ISchedulerService _schedulerService;

        public CollectorJob(ILogger<CollectorJob> logger, ISchedulerService schedulerService)
        {
            _logger = logger;
            _schedulerService = schedulerService;
        }

        public async Task Execute(IJobExecutionContext context)
        {
            _logger.LogInformation("Collector Job started");

            _logger.LogInformation("CleanUp Jobs...");
            await context.Scheduler.Clear();

            _logger.LogInformation("GetAllJobScheduleEntities started");
            var airConJobs = _schedulerService.GetAllJobScheduleEntities();

            foreach (var airConJob in airConJobs)
            {
                if (!airConJob.IsJobOn)
                    continue;

                var jobSchedule = new JobSchedule(
                    airConId: airConJob.AirConId,
                    isAirConOn: airConJob.IsAirConOn,
                    jobType: typeof(AirConJob),
                    cronExpression: airConJob.CronExpression);

                try
                {
                    var job = JobCreator.CreateJob(jobSchedule);
                    var trigger = JobCreator.CreateTrigger(jobSchedule);
                    await context.Scheduler.ScheduleJob(job, trigger, CancellationToken.None);
                }
                catch (Exception e)
                {
                    _logger.LogError($"Unable to register Job for airconId '{jobSchedule.AirConId}' with trigger {jobSchedule.CronExpression}. Error: {e.Message}");
                }

                _logger.LogInformation($"New Job added: AirConId: {airConJob.AirConId}, IsAirConOn: {airConJob.IsAirConOn}, CronExpression: {airConJob.CronExpression}");

            }
            await AddCollectorJob(context);

            await context.Scheduler.Start();
            _logger.LogInformation("Successfully executed Collector Job");
        }

        private async Task AddCollectorJob(IJobExecutionContext context)
        {
            var collectorJob = new JobSchedule(airConId: string.Empty, jobType: typeof(CollectorJob), isAirConOn: true, cronExpression: "0 * * ? * *");
            await context.Scheduler.ScheduleJob(JobCreator.CreateJob(collectorJob), JobCreator.CreateTrigger(collectorJob), CancellationToken.None);
        }
    }
}
