using System;
using Microsoft.Extensions.DependencyInjection;
using Ondo.Backend.Core.Scheduler;
using Ondo.Backend.Core.Services;
using Quartz;
using Quartz.Impl;
using Quartz.Spi;

namespace Ondo.Backend.Core
{
    public static class OndoCoreFeature
    {
        public static void ConfigureOndoCore(this IServiceCollection services)
        {
            //Add validator
            services.AddSingleton<IDeviceService, DevicesService>();
            services.AddSingleton<IAirConService, AirConService>();
            services.AddSingleton<ISchedulerService, SchedulerService>();

            // Add Quartz services
            services.AddSingleton<IJobFactory, JobFactory>();
            services.AddSingleton<ISchedulerFactory, StdSchedulerFactory>();

            // Add our job
            services.AddSingleton<AirConJob>();
            services.AddSingleton<CollectorJob>();

            services.AddCollectionJob("0 * * ? * *");

            services.AddHostedService<QuartzHostedService>();
        }

        public static void ValidateOndoCore(this IServiceProvider services)
        {
            AzureConfigurationValidator.ValidateConfiguration(services);
        }
    }
}
