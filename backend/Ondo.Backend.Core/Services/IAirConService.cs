using System.Collections.Generic;
using System.Threading.Tasks;
using Ondo.Api.Model;

namespace Ondo.Api.Services
{
    public interface IAirConService
    {
        Task<IEnumerable<AirCon>> GetAirCons();

        Task<AirCon> GetAirCon(string deviceId);

        Task ChangeAirConSetting(AirCon airCon);
    }
}
