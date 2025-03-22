using namespace QPI;

constexpr unsigned long long MAX_PROJECTS = 1024;
constexpr unsigned long long MAX_PROJECT_NAME_LENGTH = 64;
constexpr unsigned long long MAX_PROJECT_DESCRIPTION_LENGTH = 1024;

constexpr unsigned long long MAX_PROJECT_MILESTONES = 32;
constexpr unsigned long long MAX_MILESTONE_NAME_LENGTH = 64;
constexpr unsigned long long MAX_MILESTONE_DESCRIPTION_LENGTH = 1024;


// Project related arrays
Array<id, MAX_PROJECTS> mProjectCreator;
// TODO project name and description
Array<bit, MAX_PROJECTS> mProjectActive;

// Milestones related arrays
Array<uint64, MAX_PROJECTS*MAX_PROJECT_MILESTONES> mMilestoneAmount;
Array<uint32, MAX_PROJECTS*MAX_PROJECT_MILESTONES> mMilestoneDueDate;
Array<bit, MAX_PROJECTS*MAX_PROJECT_MILESTONES> mMilestonePaid;
// TODO create milestone name and description arrays

struct HM252
{
};

struct HM25 : public ContractBase
{
public:
    struct Echo_input{};
    struct Echo_output{};

    struct Burn_input{};
    struct Burn_output{};

    struct GetStats_input {};
    struct GetStats_output
    {
        uint64 numberOfEchoCalls;
        uint64 numberOfBurnCalls;
    };

private:
    uint64 numberOfEchoCalls;
    uint64 numberOfBurnCalls;

    /**
    Send back the invocation amount
    */
    PUBLIC_PROCEDURE(Echo)
        state.numberOfEchoCalls++;
        if (qpi.invocationReward() > 0)
        {
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
        }
    _

    /**
    * Burn all invocation amount
    */
    PUBLIC_PROCEDURE(Burn)
        state.numberOfBurnCalls++;
        if (qpi.invocationReward() > 0)
        {
            qpi.burn(qpi.invocationReward());
        }
    _

    PUBLIC_FUNCTION(GetStats)
        output.numberOfBurnCalls = state.numberOfBurnCalls;
        output.numberOfEchoCalls = state.numberOfEchoCalls;
    _

    REGISTER_USER_FUNCTIONS_AND_PROCEDURES

        REGISTER_USER_PROCEDURE(Echo, 1);
        REGISTER_USER_PROCEDURE(Burn, 2);

        REGISTER_USER_FUNCTION(GetStats, 1);
    _

    INITIALIZE
        state.numberOfEchoCalls = 0;
        state.numberOfBurnCalls = 0;
    _
};
