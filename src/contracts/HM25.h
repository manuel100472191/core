using namespace QPI;

constexpr unsigned long long MAX_PROJECTS = 8;
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

    struct CreateProject_input {
        uint64 title;
    };
    struct CreateProject_output {
        uint64 projectIndex;
    };

    struct CreateMilestone_input {
        uint64 projectIndex;
        uint64 title;
    };
    struct CreateMilestone_output {
        uint64 milestoneIndex;
    };

    struct CreateMilestone_output {
        uint64 currentMilestoneIndex;
    }

    struct CreateInvestment_input {
        uint64 projectIndex;
        uint64 milestoneIndex;
        uint64 amount;
    };
    struct CreateInvestment_output {
        uint64 investmentIndex;
    };

    struct CreateInvestment_locals {
        uint64 investmentIndex;
    }

    struct GetProjects_input {};
    struct GetProjects_output
    {
        uint64 numberOfProjects;
        Array<id, MAX_PROJECTS> projectCreator;
    };

    struct GetStats_input {};
    struct GetStats_output
    {
        uint64 numberOfEchoCalls;
        uint64 numberOfBurnCalls;
        Array<id, MAX_PROJECTS> projectCreator;
    };

    struct GetProjectNumbers_input {};
    struct GetProjectNumbers_output
    {
        uint64 numberOfProjects;
    };

private:
    uint64 numberOfEchoCalls;
    uint64 numberOfBurnCalls;
    uint64 numberOfProjects;

    // Project related arrays
    Array<id, MAX_PROJECTS> mProjectCreator;
    // Project name 
    Array<uint64, MAX_PROJECTS> mProjectName;
    Array<bit, MAX_PROJECTS> mProjectActive;
    // Project ->  milestones -> { id -> amounts }

    // Milestones related arrays
    Array<uint64, MAX_PROJECTS*MAX_PROJECT_MILESTONES> mMilestoneTitle;
    Array<id, MAX_PROJECTS> mNumberOfMilestonesPerProject;
    
    Array<uint32, MAX_PROJECTS*MAX_PROJECT_MILESTONES> mMilestoneDueDate; // TODO see date format and make it automatic (?)
    Array<bit, MAX_PROJECTS*MAX_PROJECT_MILESTONES> mMilestoneAchieved;
    // TODO create milestone name and description arrays
    
    // Investment related arrays
    Array<id, MAX_PROJECTS*MAX_PROJECT_MILESTONES*MAX_INVESTORS_PER_MILESTONE> mMilestoneInvestor; // Saves the investor id 
    Array<id, MAX_PROJECTS*MAX_PROJECT_MILESTONES*MAX_INVESTORS_PER_MILESTONE> mMilestoneAmountPerInvestor; // Amount related to the investor 
    Array<uint64, MAX_PROJECTS*MAX_PROJECT_MILESTONES> mNumberOfInvestorsPerMilestone;

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
        // Save the title
        state.mProjectName.set(state.numberOfProjects, input.title);
        // Set the project as active
        state.mProjectActive.set(state.numberOfProjects, 1);
        output.projectIndex = state.numberOfProjects;
        state.numberOfProjects++;
        // Set its number of milestones to 0
        state.mNumberOfMilestonesPerProject.set(state.numberOfProjects, 0);
    _

    /**
     * @output milestoneIndex
     */
    PUBLIC_PROCEDURE(CreateMilestone)
        if (qpi.invocator() != qpi.originator())
        {
            // return any leftover
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }
        if (state.mNumberOfMilestonesPerProject.get(input.projectIndex) >= MAX_PROJECT_MILESTONES) {
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }
        // Save the creator
        locals.currentMilestoneIndex = input.projectIndex*MAX_PROJECT_MILESTONES + state.mNumberOfMilestonesPerProject.get(input.projectIndex);
        state.mMilestoneTitle.set(locals.currentMilestoneIndex, input.title);
        output.milestoneIndex = state.mNumberOfMilestonesPerProject.get(input.projectIndex);
        state.mNumberOfMilestonesPerProject.set(input.projectIndex, state.mNumberOfMilestonesPerProject.get(input.projectIndex) + 1);
        // Set the number of investors of the created milestone to 0
        state.mNumberOfInvestorsPerMilestone.set(locals.currentMilestoneIndex, 0);
    _
    
    /**
     * @output investmentIndex
     */
    PUBLIC_PROCEDURE(CreateInvestment)
        if (qpi.invocator() != qpi.originator())
        {
            // return any leftover
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }

        // Save the investor
        locals.investmentIndex = input.projectIndex*MAX_PROJECT_MILESTONES*MAX_INVESTORS_PER_MILESTONE + input.milestoneIndex*MAX_INVESTORS_PER_MILESTONE;
        state.mMilestoneInvestor.set(locals.investmentIndex, qpi.invocator());
        state.mMilestoneAmountPerInvestor.set(locals.investmentIndex, input.amount);
        state.mNumberOfInvestorsPerMilestone.set(input.projectIndex*MAX_PROJECT_MILESTONES + input.milestoneIndex, state.mNumberOfInvestorsPerMilestone.get(input.projectIndex*MAX_PROJECT_MILESTONES + input.milestoneIndex) + 1);
        output.investmentIndex = locals.investmentIndex;
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

    PUBLIC_FUNCTION(GetProjects)
        output.numberOfProjects = state.numberOfProjects;
        output.projectCreator = state.mProjectCreator;
    _

    PUBLIC_FUNCTION(GetProjectNumbers)
        output.numberOfProjects = state.numberOfProjects;
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
        REGISTER_USER_PROCEDURE(CreateMilestone, 4);
        REGISTER_USER_PROCEDURE(CreateInvestment, 5);

        REGISTER_USER_FUNCTION(GetStats, 1);
        REGISTER_USER_FUNCTION(GetProjects, 2);
        REGISTER_USER_FUNCTION(GetProjectNumbers, 3);
    _

    INITIALIZE
        state.numberOfEchoCalls = 0;
        state.numberOfBurnCalls = 0;
        for (uint32 i = 0; i < MAX_PROJECTS; i++)
        {
            state.mProjectCreator.set(i, NULL_ID);
            state.mProjectActive.set(i, 0);
        }
        state.numberOfProjects = 0;
    _
};
