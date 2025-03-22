using namespace QPI;

constexpr unsigned long long MAX_PROJECTS = 1024;
constexpr unsigned long long MAX_PROJECT_NAME_LENGTH = 64;
constexpr unsigned long long MAX_PROJECT_DESCRIPTION_LENGTH = 1024;
constexpr unsigned long long MAX_PROJECT_INVESTORS = 32;
constexpr unsigned long long MAX_PROJECT_MILESTONES = 32;

constexpr unsigned long long MAX_MILESTONE_NAME_LENGTH = 64;
constexpr unsigned long long MAX_MILESTONE_DESCRIPTION_LENGTH = 1024;
constexpr unsigned long long MAX_INVESTORS_PER_MILESTONE = 32;


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

    struct CreateProject_input {};
    struct CreateProject_output {
        uint64 projectIndex;
    };

    struct GetStats_input {};
    struct GetStats_output
    {
        uint64 numberOfEchoCalls;
        uint64 numberOfBurnCalls;
        Array<id, MAX_PROJECTS> projectCreator;
    };

private:
    uint64 numberOfEchoCalls;
    uint64 numberOfBurnCalls;
    uint64 numberOfProjects;

    // Project related arrays
    Array<id, MAX_PROJECTS> mProjectCreator;
    // TODO project name and description
    Array<bit, MAX_PROJECTS> mProjectActive;
    // Project ->  milestones -> { id -> amounts }

    // Milestones related arrays
    Array<id, MAX_PROJECTS*MAX_PROJECT_MILESTONES*MAX_INVESTORS_PER_MILESTONE> mMilestoneInvestor; // Saves the investor id 
    Array<id, MAX_PROJECTS*MAX_PROJECT_MILESTONES*MAX_INVESTORS_PER_MILESTONE> mMilestoneAmountPerInvestor; // Amount related to the investor 

    Array<uint32, MAX_PROJECTS*MAX_PROJECT_MILESTONES> mMilestoneDueDate; // TODO see date format and make it automatic (?)
    Array<bit, MAX_PROJECTS*MAX_PROJECT_MILESTONES> mMilestoneAchieved;
    // TODO create milestone name and description arrays

    /**
     * @output projectIndex
     */
    PUBLIC_PROCEDURE(CreateProject)
        if (qpi.invocator() != qpi.originator())
        {
            // return any leftover
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }
        if (state.numberOfProjects >= MAX_PROJECTS) {
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }
        // Save the creator
        state.mProjectCreator.set(state.numberOfProjects, qpi.invocator());
        output.projectIndex = state.numberOfProjects;
        state.numberOfProjects++;
    _

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
        output.projectCreator = state.mProjectCreator;
    _

    REGISTER_USER_FUNCTIONS_AND_PROCEDURES
        
        REGISTER_USER_PROCEDURE(Echo, 1);
        REGISTER_USER_PROCEDURE(Burn, 2);
        REGISTER_USER_PROCEDURE(CreateProject, 3);

        REGISTER_USER_FUNCTION(GetStats, 1);
    _

    INITIALIZE
        state.numberOfEchoCalls = 0;
        state.numberOfBurnCalls = 0;
        for (uint32 i = 0; i < MAX_PROJECTS; i++)
        {
            state.mProjectCreator.set(i, NULL_ID);
            state.mProjectActive.set(i, 0);
        }
    _
};
