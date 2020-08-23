using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.Azure.Cosmos.Table;
using Microsoft.Extensions.Options;
using Ondo.Backend.Core.Configuration;
using Ondo.Backend.Core.Model;

namespace Ondo.Backend.Core.Services
{
    public class SchedulerService : ISchedulerService
    {
        private readonly AzureConfiguration _azureConfiguration;

        public SchedulerService(IOptions<AzureConfiguration> azureConfiguration)
        {
            _azureConfiguration = azureConfiguration.Value;
        }
        
        public IEnumerable<JobScheduleEntity> GetAllJobScheduleEntities()
        {
            var account = CloudStorageAccount.Parse(_azureConfiguration.StorageConnectionString);
            var client = account.CreateCloudTableClient(new TableClientConfiguration());
            var table = client.GetTableReference("JobScheduler");

            var query = new TableQuery<JobScheduleEntity>();
            var result = table.ExecuteQuery(query).ToList();

            return result;
        }

        public async Task InsertOrMergeTableEntityAsync(JobScheduleEntity js)
        {
            var account = CloudStorageAccount.Parse(_azureConfiguration.StorageConnectionString);
            var client = account.CreateCloudTableClient(new TableClientConfiguration());
            var table = client.GetTableReference("JobScheduler");

            TableOperation insertOperation = TableOperation.InsertOrMerge(js);
            await table.ExecuteAsync(insertOperation);
        }

        public async Task<JobScheduleEntity> GetJobsScheduleEntity(string partitionKey, string rowKey)
        {
            var account = CloudStorageAccount.Parse(_azureConfiguration.StorageConnectionString);
            var client = account.CreateCloudTableClient(new TableClientConfiguration());
            var table = client.GetTableReference("JobScheduler");

            var tableOperation = TableOperation.Retrieve<JobScheduleEntity>(partitionKey, rowKey);

            var result = await table.ExecuteAsync(tableOperation);

            return result.Result as JobScheduleEntity; 
        }

        public async Task DeleteTableEntityAsync(string partitionKey, string rowKey)
        {
            var account = CloudStorageAccount.Parse(_azureConfiguration.StorageConnectionString);
            var client = account.CreateCloudTableClient(new TableClientConfiguration());
            var table = client.GetTableReference("JobScheduler");

            var jobScheduleEntity = await GetJobsScheduleEntity(partitionKey, rowKey);

            var tableOperation = TableOperation.Delete(jobScheduleEntity);

            await table.ExecuteAsync(tableOperation);
        }
    }
}
