#! /usr/bin/env python3

# SPDX-FileCopyrightText: 2021 microDev
#
# SPDX-License-Identifier: MIT

# GraphQL Query

QUERY_COMMITS = """
query ($owner: String!, $name: String!, $pullNumber: Int!, $commitsPerPage: Int!, $beforeCommit: String) {
  repository(owner: $owner, name: $name) {
    pullRequest(number: $pullNumber) {
      commits(last: $commitsPerPage, before: $beforeCommit) {
        totalCount
        pageInfo {
          startCursor
          hasPreviousPage
        }
        nodes {
          commit {
            checkSuites(first: 3) {
              nodes {
                conclusion
                workflowRun {
                  workflow {
                    name
                  }
                }
                id
              }
              totalCount
            }
            oid
          }
        }
      }
    }
  }
}
"""

QUERY_CHECKRUNS = """
query ($checkSuiteID: ID!,
       $afterFailedRun: String, $afterIncompleteRun: String,
       $includeFailedRuns: Boolean!, $includeIncompleteRuns: Boolean!) {
  node(id: $checkSuiteID) {
    ... on CheckSuite {
      failedRuns: checkRuns(
        first: 100
        after: $afterFailedRun
        filterBy: {checkType: LATEST, conclusions: [ACTION_REQUIRED, TIMED_OUT, CANCELLED, FAILURE, NEUTRAL, STARTUP_FAILURE]}
      ) @include(if: $includeFailedRuns) {
        nodes {
          name
        }
        pageInfo {
          endCursor
          hasNextPage
        }
      }
      incompleteRuns: checkRuns(
        first: 100
        after: $afterIncompleteRun
        filterBy: {checkType: LATEST, statuses: [QUEUED, IN_PROGRESS, WAITING, PENDING, REQUESTED]}
      ) @include(if: $includeIncompleteRuns) {
        nodes {
          name
        }
        pageInfo {
          endCursor
          hasNextPage
        }
      }
    }
  }
}
"""


import os
import re
import json
import requests


query_variables_commits = {
    "owner": "",
    "name": "",
    "pullNumber": int(os.environ["PULL"]),
    "commitsPerPage": 20,
    "beforeCommit": None,
}


query_variables_checkruns = {
    "checkSuiteID": "",
    "afterFailedRun": None,
    "afterIncompleteRun": None,
    "includeFailedRuns": True,
    "includeIncompleteRuns": True,
}


headers = {"Authorization": f"Bearer {os.environ['GITHUB_TOKEN']}"}


class Query:
    def __init__(self, query, variables={}, headers={}):
        self.query = query
        self.variables = variables
        self.headers = headers

    def paginate(self, page_info, name):
        has_page = (
            page_info["hasNextPage"] if name.startswith("after") else page_info["hasPreviousPage"]
        )
        if has_page:
            self.variables[name] = (
                page_info["endCursor"] if name.startswith("after") else page_info["startCursor"]
            )
        return has_page

    def fetch(self):
        request = requests.post(
            "https://api.github.com/graphql",
            json={"query": self.query, "variables": self.variables},
            headers=self.headers,
        )
        if request.status_code == 200:
            return request.json()
        else:
            raise Exception("Query Failed: {}".format(request.status_code))


def set_output(name, value):
    if "GITHUB_OUTPUT" in os.environ:
        with open(os.environ["GITHUB_OUTPUT"], "at") as f:
            print(f"{name}={value}", file=f)
    else:
        print(f"Would set GitHub actions output {name} to '{value}'")


def get_commit_and_checksuite(query_commits):
    commits = query_commits.fetch()["data"]["repository"]["pullRequest"]["commits"]

    if commits["totalCount"] > 0:
        for commit in reversed(commits["nodes"]):
            commit = commit["commit"]
            commit_sha = commit["oid"]
            if commit_sha == os.environ["EXCLUDE_COMMIT"]:
                continue
            checksuites = commit["checkSuites"]
            if checksuites["totalCount"] > 0:
                for checksuite in checksuites["nodes"]:
                    if checksuite["workflowRun"]["workflow"]["name"] == "Build CI":
                        return [
                            commit_sha,
                            checksuite["id"] if checksuite["conclusion"] != "SUCCESS" else None,
                        ]
        else:
            if query_commits.paginate(commits["pageInfo"], "beforeCommit"):
                return get_commit_and_checksuite(query_commits)

    return [None, None]


def append_runs_to_list(runs, list):
    regex_matrix = re.compile("^build-[^ ]+")
    regex_board = re.compile("\([^ ]+\)$")
    for run in runs["nodes"]:
        name = run["name"]
        res_matrix = regex_matrix.search(name)
        if res_matrix:
            matrix = res_matrix.group()
            if matrix not in list:
                list[matrix] = []
            list[matrix].append(regex_board.search(name).group()[1:-1])


def get_bad_checkruns(query_checkruns, list={}):
    checkruns = query_checkruns.fetch()["data"]["node"]
    run_types = ["failed", "incomplete"]
    paginate = False

    for run_type in run_types:
        run_type_camel = run_type.capitalize() + "Run"
        run_type = run_type + "Runs"

        append_runs_to_list(checkruns[run_type], list)

        if query_checkruns.paginate(checkruns[run_type]["pageInfo"], "after" + run_type_camel):
            query_checkruns.variables["include" + run_type_camel] = True
            paginate = True

    return get_bad_checkruns(query_checkruns, list) if paginate else list


def main():
    query_commits = Query(QUERY_COMMITS, query_variables_commits, headers)
    query_commits.variables["owner"], query_commits.variables["name"] = os.environ["REPO"].split(
        "/"
    )

    commit, checksuite = get_commit_and_checksuite(query_commits)

    if checksuite is None:
        if commit is None:
            print("No checkSuites found -> Abort")
        else:
            set_output("commit", commit)
        quit()

    query_checkruns = Query(QUERY_CHECKRUNS, query_variables_checkruns, headers)
    query_checkruns.variables["checkSuiteID"] = checksuite

    checkruns = get_bad_checkruns(query_checkruns)

    if len(checkruns) == 0:
        print("No checkRuns found -> Abort")
        quit()

    set_output("commit", commit)
    set_output("checkruns", json.dumps(checkruns))


if __name__ == "__main__":
    main()
