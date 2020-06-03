using IoTHubTrigger = Microsoft.Azure.WebJobs.EventHubTriggerAttribute;

using Microsoft.Azure.WebJobs;
using Microsoft.Azure.EventHubs;
using System.Text;
using System.Net.Http;
using Microsoft.Extensions.Logging;

namespace Ondo.Backend.Functions
{
    public static class ProcessInboundEvents
    {
        private static HttpClient client = new HttpClient();

        [FunctionName("ProcessInboundEvents")]
        public static void Run([IoTHubTrigger("messages/events", Connection = "ConnectionString")]EventData message, ILogger log)
        {
            var messageBody = Encoding.UTF8.GetString(message.Body.Array);

            var deviceId = message.SystemProperties["iothub-connection-device-id"];
            var enqueueDateTime = message.SystemProperties.EnqueuedTimeUtc;

            
            log.LogInformation($"[{deviceId}] {enqueueDateTime.ToLocalTime()} {messageBody}");
        }
    }
}