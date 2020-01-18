using System.Collections.Generic;
using System.Threading.Tasks;
using Ondo.Backend.Core.Model;

namespace Ondo.Backend.Core.Services
{
    public interface IAirConService
    {
        Task<IEnumerable<AirCon>> GetAirCons();

        Task<AirCon> GetAirCon(string deviceId);

        Task ChangeAirConSetting(AirCon airCon);
    }
}
