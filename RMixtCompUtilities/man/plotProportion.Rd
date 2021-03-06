% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/PLOT_barplot.R
\name{plotProportion}
\alias{plotProportion}
\title{Plot the mixture's proportions}
\usage{
plotProportion(output, pkg = c("ggplot2", "plotly"), ...)
}
\arguments{
\item{output}{object returned by function \emph{mixtCompLearn}}

\item{pkg}{"ggplot2" or "plotly". Package used to plot}

\item{...}{arguments to be passed to plot_ly}
}
\description{
Plot the mixture's proportions
}
\examples{
\donttest{
data(simData)
 
# define the algorithm's parameters
algo <- createAlgo()

# keep only 3 variables
model <- simData$model$unsupervised[c("Gaussian1", "Poisson1", "Categorical1")]

# run RMixtComp in unsupervised clustering mode + data as matrix
res <- mixtCompLearn(simData$dataLearn$matrix, model, algo, nClass = 2:4)

# plot
plotProportion(res)
plotProportion(res$res[[2]])
}

}
\seealso{
Other plot: \code{\link{heatmapClass}},
  \code{\link{heatmapTikSorted}}, \code{\link{heatmapVar}},
  \code{\link{histMisclassif}},
  \code{\link{plot.MixtComp}},
  \code{\link{plotConvergence}}, \code{\link{plotCrit}},
  \code{\link{plotDataBoxplot}}, \code{\link{plotDataCI}},
  \code{\link{plotDiscrimClass}},
  \code{\link{plotDiscrimVar}},
  \code{\link{plotParamConvergence}}
}
\author{
Quentin Grimonprez
}
\concept{plot}
