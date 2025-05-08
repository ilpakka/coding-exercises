class Solution {
public:
    bool assign(int j, vector<int> &tasks, vector<int> &workers, int pills, int strength) {
        multiset<int> availableWorkers(workers.end() - j, workers.end());
        for (int i = j - 1; i >= 0; i--) {
            auto it = availableWorkers.lower_bound(tasks[i]);
            if (it != availableWorkers.end()) {
                availableWorkers.erase(it);
            } else {
                if (pills == 0) {
                    return false;
                }
                it = availableWorkers.lower_bound(tasks[i] - strength);
                if (it == availableWorkers.end()) {
                    return false;
                }
                availableWorkers.erase(it);
                pills--;
            }
        }
        return true;
    }
    int maxTaskAssign(vector<int>& tasks, vector<int>& workers, int pills, int strength) {
        // sort tasks and workers
        sort(tasks.begin(), tasks.end());
        sort(workers.begin(), workers.end());

        int low = 0;
        int high = min(tasks.size(), workers.size()); // whichever is lower
        int count = 0;

        // binary search
        while (low <= high) {
            int mid = (low + high) / 2;
            if (assign(mid, tasks, workers, pills, strength)) { // can assign
                count = mid;
                low = mid + 1;
            } else { // try lower
                high = mid - 1;
            }
        }
        return count;
    }
};