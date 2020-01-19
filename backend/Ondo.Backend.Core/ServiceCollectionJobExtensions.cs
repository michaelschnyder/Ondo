using Microsoft.Extensions.DependencyInjection;
using Ondo.Backend.Core.Scheduler;

namespace Ondo.Mvc
{
    public static class ServiceCollectionJobExtensions
    {
        public static void AddJob(this IServiceCollection services, string AirConId, bool turnOn, string CronExpression)
        {
            services.AddSingleton(sp =>
            {
                return new JobSchedule(
                    airConId: AirConId,
                    turnOn: turnOn,
                    jobType: typeof(AirConJob),
                    cronExpression: CronExpression);
            });
        }
    }
}