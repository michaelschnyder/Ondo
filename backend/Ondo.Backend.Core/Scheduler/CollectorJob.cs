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
        private readonly ILogger<AirConJob> _logger;
        private readonly ISchedulerService _schedulerService;

        public CollectorJob(ILogger<AirConJob> logger, ISchedulerService schedulerService)
        {
            _logger = logger;
            _schedulerService = schedulerService;
        }

        public async Task Execute(IJobExecutionContext context)
        {
            await RemoveExistingAirConJobs(context);

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
            await context.Scheduler.Start(CancellationToken.None);
            _logger.LogInformation("Successfully executed Collector Job");
        }

        private static async Task RemoveExistingAirConJobs(IJobExecutionContext context)
        {
            var currentlyExecutingJobs = await context.Scheduler.GetCurrentlyExecutingJobs();

            foreach (var job in currentlyExecutingJobs)
            {
                if (job.JobInstance is CollectorJob)
                {
                    continue;
                }
                await context.Scheduler.DeleteJob(job.JobDetail.Key);
            }
        }
    }
}
