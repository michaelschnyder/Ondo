using System;
using Microsoft.Azure.Devices.Common;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using Ondo.Backend.Core;

namespace Ondo.Backend
{
    public class AzureConfigurationValidator
    {
        public static void ValidateConfiguration(IServiceProvider serviceProvider)
        {
            var azureConfiguration = serviceProvider.GetService<IOptions<AzureConfiguration>>();

            var logger = serviceProvider.GetService<ILogger<AzureConfigurationValidator>>();
            if (azureConfiguration == null || azureConfiguration.Value == null ||
                azureConfiguration.Value.IoTHubConnectionString.IsNullOrWhiteSpace())
            {
                logger.LogError("Unable to find AzureConfiguration or IoT Hub connection string.");
            }
            else
            {
                logger.LogInformation($"Application started. Configuration is {azureConfiguration.Value.IoTHubConnectionString}");
            }
        }
    }
}
