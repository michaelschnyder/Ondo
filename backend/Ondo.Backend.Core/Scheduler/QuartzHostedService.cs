using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Quartz;
using Quartz.Spi;

namespace Ondo.Backend.Core.Scheduler
{
    public class QuartzHostedService : IHostedService
    {
        private readonly ISchedulerFactory _schedulerFactory;
        private readonly IJobFactory _jobFactory;
        private readonly IEnumerable<JobSchedule> _jobSchedules;
        private readonly ILogger<QuartzHostedService> _logger;

        public QuartzHostedService(ISchedulerFactory schedulerFactory, IJobFactory jobFactory, IEnumerable<JobSchedule> jobSchedules, ILogger<QuartzHostedService> logger)
        {
            _schedulerFactory = schedulerFactory;
            _jobSchedules = jobSchedules;
            _logger = logger;
            _jobFactory = jobFactory;
        }

        public IScheduler Scheduler { get; set; }

        public async Task StartAsync(CancellationToken cancellationToken)
        {
            _logger.LogInformation($"Started {nameof(QuartzHostedService)} and processing registered Jobs from DI");
            Scheduler = await _schedulerFactory.GetScheduler(cancellationToken);
            Scheduler.JobFactory = _jobFactory;

            foreach (var jobSchedule in _jobSchedules)
            {
                try
                {
                    var job = JobCreator.CreateJob(jobSchedule);
                    var trigger = JobCreator.CreateTrigger(jobSchedule);
                    await Scheduler.ScheduleJob(job, trigger, cancellationToken);
                }
                catch (Exception e)
                {
                    _logger.LogError($"Unable to register Job for airconId '{jobSchedule.AirConId}' with trigger {jobSchedule.CronExpression}. Error: {e.Message}");
                }
            }

            _logger.LogInformation($"Starting Scheduler after {_jobSchedules.Count()} jobs have been registered.");

            await Scheduler.Start(cancellationToken);
        }

        public async Task StopAsync(CancellationToken cancellationToken)
        {
            await Scheduler?.Shutdown(cancellationToken);
        }
    }
}
