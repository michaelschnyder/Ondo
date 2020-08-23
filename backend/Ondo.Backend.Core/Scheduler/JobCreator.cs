using System;
using System.Runtime.InteropServices;
using Quartz;

namespace Ondo.Backend.Core.Scheduler
{
    public static class JobCreator
    {
        public static IJobDetail CreateJob(JobSchedule schedule)
        {
            var jobType = schedule.JobType;
            return JobBuilder
                .Create(jobType)
                .WithIdentity(jobType.FullName + schedule.AirConId + schedule.CronExpression + Guid.NewGuid())
                .WithDescription(jobType.Name)
                .UsingJobData("AirConId", schedule.AirConId)
                .UsingJobData("IsAirConOn", schedule.IsAirConOn)
                .Build();

        }

        public static ITrigger CreateTrigger(JobSchedule schedule)
        {
            return TriggerBuilder
                .Create()
                .WithIdentity($"{schedule.JobType.FullName + schedule.AirConId}.trigger-" + schedule.CronExpression + Guid.NewGuid())
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
            return TimeZoneInfo.FindSystemTimeZoneById("Asia/Singapore");
        }
    }
}
