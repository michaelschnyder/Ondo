using Microsoft.Extensions.DependencyInjection;
using Ondo.Backend.Core.Scheduler;

namespace Ondo.Backend.Core
{
    public static class ServiceCollectionJobExtensions
    {
        public static void AddCollectionJob(this IServiceCollection services, string cronExpression)
        {
            services.AddSingleton(sp =>
            {
                return new JobSchedule(
                    airConId: string.Empty,
                    isAirConOn: true,
                    jobType: typeof(CollectorJob),
                    cronExpression: cronExpression);
            });
        }
    }
}