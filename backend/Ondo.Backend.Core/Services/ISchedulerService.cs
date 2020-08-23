using Ondo.Backend.Core.Model;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Ondo.Backend.Core.Services
{
    public interface ISchedulerService 
    {
        IEnumerable<JobScheduleEntity> GetAllJobScheduleEntities();

        Task InsertOrMergeTableEntityAsync(JobScheduleEntity js);

        Task<JobScheduleEntity> GetJobsScheduleEntity(string partitionKey, string rowKey);

        Task DeleteTableEntityAsync(string partitionKey, string rowKey);
    }
}
