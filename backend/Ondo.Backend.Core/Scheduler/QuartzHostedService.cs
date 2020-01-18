using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
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
            Scheduler = await _schedulerFactory.GetScheduler(cancellationToken);
            Scheduler.JobFactory = _jobFactory;

            foreach (var jobSchedule in _jobSchedules)
            {

                try
                {
                    var job = CreateJob(jobSchedule);
                    var trigger = CreateTrigger(jobSchedule);
                    await Scheduler.ScheduleJob(job, trigger, cancellationToken);

                }
                catch (Exception e)
                {
                    _logger.LogError($"Unable to register Job for airconId '{jobSchedule.AirConId}' with trigger {jobSchedule.CronExpression}. Error: {e.Message}");
                }
            }

            await Scheduler.Start(cancellationToken);
        }

        public async Task StopAsync(CancellationToken cancellationToken)
        {
            await Scheduler?.Shutdown(cancellationToken);
        }

        private static IJobDetail CreateJob(JobSchedule schedule)
        {
            var jobType = schedule.JobType;
            return JobBuilder
                .Create(jobType)
                .WithIdentity(jobType.FullName + schedule.AirConId + schedule.CronExpression)
                .WithDescription(jobType.Name)
                .UsingJobData("AirConId", schedule.AirConId)
                .UsingJobData("TurnOn", schedule.TurnOn)
                .Build();
            
        }

        private static ITrigger CreateTrigger(JobSchedule schedule)
        {
            return TriggerBuilder
                .Create()
                .WithIdentity($"{schedule.JobType.FullName + schedule.AirConId}.trigger-" + schedule.CronExpression)
                .WithCronSchedule(schedule.CronExpression, builder => builder.InTimeZone(GetSingaporeTimeZone()))
                .WithDescription(schedule.CronExpression)
                .Build();
        }

        private static TimeZoneInfo GetSingaporeTimeZone()
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                return TimeZoneInfo.FindSystemTimeZoneById("Singapore Standard Time");
            }
            else
            {
                return TimeZoneInfo.FindSystemTimeZoneById("Asia/Singapore");
            }
        }
    }
}
